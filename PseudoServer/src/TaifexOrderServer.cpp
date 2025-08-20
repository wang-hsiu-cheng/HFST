#include "TaifexOrderServer.h"

int Session::RecvSinglePacket(char* outputBuffer, unsigned int flags)
{
    while(1) 
    {
        // printf("[%d] 0 m_currentPacketSize = %d, m_recvBufferLeftSize = %d\n", GetSessionNum(), m_currentPacketSize, m_recvBufferLeftSize);
        if(m_recvBufferLeftSize == 0)
        {
            int recvSize = recv(m_sockfd, m_recvBuffer.data(), MAX_SIZE, flags);
            // printf("[%d] recv buffer left size is 0, recvSize = %d\n", GetSessionNum(), recvSize);
            if(recvSize <= 0)
            {
                return recvSize;
            }
            m_recvBufferLeftSize = recvSize;
            m_cur_recvBufferPtr = m_recvBuffer.data();
        }
        // printf("[%d] 1 m_currentPacketSize = %d, m_recvBufferLeftSize = %d\n", GetSessionNum(), m_currentPacketSize, m_recvBufferLeftSize);
        if(m_currentPacketSize == 0)
        {
            if(m_recvBufferLeftSize >= (int)sizeof(FIXhdr_t))
            {
                FIXhdr_t* hdr = reinterpret_cast<FIXhdr_t*>(m_cur_recvBufferPtr);
                m_currentPacketSize = STR_TO_INT(hdr->BodyLength) + 3;
            }
        }
        // printf("[%d] 2 m_currentPacketSize = %d, m_recvBufferLeftSize = %d\n", GetSessionNum(), m_currentPacketSize, m_recvBufferLeftSize);
        if(m_recvBufferLeftSize >= m_currentPacketSize && m_currentPacketSize != 0)
        {
            std::memcpy(outputBuffer, m_cur_recvBufferPtr, m_currentPacketSize);
            m_recvBufferLeftSize -= m_currentPacketSize;
            m_cur_recvBufferPtr += m_currentPacketSize;
            int retPacketSize = m_currentPacketSize;
            m_currentPacketSize = 0;
            // printf("[%d] A retPacketSize = %d, m_recvBufferLeftSize = %d\n", GetSessionNum(), retPacketSize, m_recvBufferLeftSize);
            return retPacketSize;
        }
        else
        {
            // printf("[%d] B retPacketSize = %d, m_recvBufferLeftSize = %d\n", GetSessionNum(), m_currentPacketSize, m_recvBufferLeftSize);
            std::memcpy(m_recvBuffer.data(), m_cur_recvBufferPtr, m_recvBufferLeftSize);

            int recvSize = recv(m_sockfd, m_recvBuffer.data() + m_recvBufferLeftSize, MAX_SIZE - m_recvBufferLeftSize, flags);
            // printf("[%d] QQQ1 recv recvSize = %d, retPacketSize = %d, m_recvBufferLeftSize = %d\n", GetSessionNum(), recvSize, m_currentPacketSize, m_recvBufferLeftSize);
            if(recvSize <= 0)
            {
                return recvSize;
            }
            m_recvBufferLeftSize += recvSize;
            m_cur_recvBufferPtr = m_recvBuffer.data();
            // printf("[%d] QQQ2 recv recvSize = %d, retPacketSize = %d, m_recvBufferLeftSize = %d\n", GetSessionNum(), recvSize, m_currentPacketSize, m_recvBufferLeftSize);
        }
        // printf("[%d] 3 m_currentPacketSize = %d, m_recvBufferLeftSize = %d\n", GetSessionNum(), m_currentPacketSize, m_recvBufferLeftSize);
    }

    return 0;
}

int Session::SendPacket(uint8_t *buf, int len)
{
    int sentSize = send(m_sockfd, buf, len, 0);
    return sentSize;
}

/************************************ TaifexOrderServer ************************************/

int TaifexOrderServer::Start()
{
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(m_server_ip.c_str());
    serverAddr.sin_port = htons(m_port);
    if (bind(m_sockfd, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0) 
    {
        std::cout << "Bind failed: " << strerror(errno) << std::endl;
        std::exit(EXIT_FAILURE);
    }

    if (listen(m_sockfd, m_expectedSessions) < 0) 
    {
        std::cout << "Listen failed: " << strerror(errno) << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on " << m_server_ip << ":" << m_port << " with " << m_expectedSessions << " sessions." << std::endl;

    for (int i = 0; i < m_expectedSessions; i++) 
    {
        sockaddr_in clientAddr{};
        socklen_t addrLen = sizeof(clientAddr);
        printf("Waiting for connection...\n");fflush(stdout);
        int clientSockfd = accept(m_sockfd, reinterpret_cast<sockaddr*>(&clientAddr), &addrLen);
        if (clientSockfd < 0) 
        {
            std::cout << "Accept failed: " << strerror(errno) << std::endl;
            continue;
        }
        printf("Connection accepted.\n");fflush(stdout);
        // 將連線封裝成 Session 物件
        auto session = std::make_shared<Session>(clientSockfd, i, m_port);
        {
            std::lock_guard<std::mutex> lock(m_sessionMutex);
            m_vec_sessions.push_back(session);
        }

        //create thread by session
        m_vec_threads.emplace_back([this, session, clientAddr]() 
        {
            // start login
            char buffer[4096] = {0};
            int recvSize = 0;
            int recvMsgError = 0;
            auto start = std::chrono::steady_clock::now();
            std::cout << "[Session " << session->GetSessionNum() << "] Connected from " << inet_ntoa(clientAddr.sin_addr) << std::endl;
            m_hdr.MsgSeqNum = -1;
            currentState = State::Logging;
            currentOrderState = OrderState::Idle;

            // use a delay thread to check if logno finished in 60 seconds
            std::thread delayed_thread([this, session]()
            {
                if (!isLogon)
                    std::this_thread::sleep_for(std::chrono::milliseconds(60000));
                if (!isLogon) {
                    std::cerr << "[Session " << session->GetSessionNum() << "] Connection error." << std::endl;
                    return 0;
                }
                else {
                    char buffer[32] = {0};
                    while(1)
                    {
                        EnableHeartBeat();
                        sleep(m_heartbeat_interval_sec * 2);
                        if (m_en_hb) {
                            m_messenger.Make0((uint8_t*)buffer, m_hdr.TargetCompID, 94);
                            session->SendPacket((uint8_t*)buffer, sizeof(FIX_0_t));
                            {
                                std::lock_guard<std::mutex> lock(m_sessionMutex);
                                m_sent_0_cnt++;
                            }
                            printf("[SERVER][heartbeat] port %d, session %d, send 0(%d)\n", m_port, session->GetSessionNum(), m_sent_0_cnt);
                        }
                        if (m_rcv) {
                            m_messenger.Make1((uint8_t*)buffer, m_hdr.TargetCompID, 94);
                            session->SendPacket((uint8_t*)buffer, sizeof(FIX_1_t));
                        }
                    }
                }
            });
            delayed_thread.detach();

            while (1) {
                recvSize = session->RecvSinglePacket(buffer);
                m_rcv = false;
                if (recvSize <= 0) 
                {
                    std::cerr << "[Session " << session->GetSessionNum() << "] Connection error." << std::endl;
                    return 0;
                }
                recvMsgError = m_messenger.SearchMsgType(buffer, m_hdr, m_2_data, m_3_data);
                
                switch (currentState) {
                    case State::Logging:
                        if (m_hdr.MessageType == order.FIXMsgType["Logon"] && !recvMsgError) {
                            m_rcv = true;
                            if (m_messenger.ReceiveA((uint8_t*)buffer, &m_A_data)) {
                                SetHeartBeatIntervalInSec(m_A_data.HeartBtInt);
                                m_messenger.MakeA((uint8_t*)buffer, m_hdr.TargetCompID, 94);
                                session->SendPacket((uint8_t*)buffer, sizeof(FIX_A_t));
                                printf("[SERVER] port %d, session %d, send A \n", m_port, session->GetSessionNum());
                                isLogon = true;
                                currentState = State::Ordering;
                                std::cout << "Ordering" << std::endl;
                            } else {
                                m_messenger.Make5((uint8_t*)buffer, m_hdr.TargetCompID, 94);
                                session->SendPacket((uint8_t*)buffer, sizeof(FIX_5_t));
                                start = std::chrono::steady_clock::now();
                                currentState = State::SendingLogout;
                                std::cout << "SendingLogout" << std::endl;
                            }
                        }
                        else if ((m_hdr.MessageType == order.FIXMsgType["Heartbeat"] && !recvMsgError)) {
                            m_rcv = true;
                            currentState = State::Logging;
                            std::cout << "Logging" << std::endl;
                        }
                        else if (m_hdr.MessageType == order.FIXMsgType["TestRequest"] && !recvMsgError) {
                            m_rcv = true;
                            m_messenger.Make0((uint8_t*)buffer, m_hdr.TargetCompID, 94);
                            session->SendPacket((uint8_t*)buffer, sizeof(FIX_0_t));
                            currentState = State::Logging;
                            std::cout << "Logging" << std::endl;
                        }
                        else if (m_hdr.MessageType == order.FIXMsgType["ResendRequest"] && !recvMsgError) {
                            m_rcv = true;
                            m_messenger.Make2resp((uint8_t*)buffer, m_hdr.TargetCompID, 94);
                            session->SendPacket((uint8_t*)buffer, sizeof(FIX_2_t));
                            currentState = State::Logging;
                            std::cout << "Logging" << std::endl;
                        }
                        else if (m_hdr.MessageType == order.FIXMsgType["RejectSession"] && !recvMsgError) {
                            m_rcv = true;
                            m_messenger.Make3resp((uint8_t*)buffer, m_hdr.TargetCompID, 94);
                            session->SendPacket((uint8_t*)buffer, sizeof(FIX_3_t));
                            currentState = State::Logging;
                            std::cout << "Logging" << std::endl;      
                        }
                        else if (m_hdr.MessageType == order.FIXMsgType["Logout"]) {
                            m_rcv = true;
                            m_messenger.Make5((uint8_t*)buffer, m_hdr.TargetCompID, 94);
                            session->SendPacket((uint8_t*)buffer, sizeof(FIX_5_t));
                            currentState = State::Idle;
                            std::cout << "Logoutted" << std::endl;
                            return 1;
                        }
                        else if (recvMsgError == 2) {
                            m_rcv = true;
                            m_messenger.Make2((uint8_t*)buffer, &m_2_data, m_hdr.TargetCompID, 94);
                            session->SendPacket((uint8_t*)buffer, sizeof(FIX_2_t));
                            printf("[SERVER] port %d, session %d, send 2 \n", m_port, session->GetSessionNum());
                            currentState = State::Logging;
                            std::cout << "Logging" << std::endl;
                        }
                        else if (recvMsgError == 3) {
                            m_rcv = true;
                            m_messenger.Make3((uint8_t*)buffer, &m_3_data, m_hdr.TargetCompID, 94);
                            session->SendPacket((uint8_t*)buffer, sizeof(FIX_3_t));
                            printf("[SERVER] port %d, session %d, send 3 \n", m_port, session->GetSessionNum());
                            currentState = State::Logging;
                            std::cout << "Logging" << std::endl;
                        }
                        else if (recvMsgError == 5) {
                            m_rcv = true;
                            m_messenger.Make5((uint8_t*)buffer, m_hdr.TargetCompID, 94);
                            session->SendPacket((uint8_t*)buffer, sizeof(FIX_5_t));
                            start = std::chrono::steady_clock::now();
                            currentState = State::SendingLogout;
                            std::cout << "SendingLogout" << std::endl;
                        }
                        else {
                            m_messenger.Make5((uint8_t*)buffer, m_hdr.TargetCompID, 94);
                            session->SendPacket((uint8_t*)buffer, sizeof(FIX_5_t));
                            start = std::chrono::steady_clock::now();
                            currentState = State::SendingLogout;
                            std::cout << "error state order, SendingLogout" << std::endl;
                        }
                        break;
                    case State::Ordering:
                        if (m_hdr.MessageType == order.FIXMsgType["NewOrder"] ||
                            m_hdr.MessageType == order.FIXMsgType["OrderCancel"] ||
                            m_hdr.MessageType == order.FIXMsgType["OrderCancelRequest"] ||
                            m_hdr.MessageType == order.FIXMsgType["OrderStatus"]) {
                            m_rcv = true;
                            bool isOrderAccepted = false;
                            switch (currentOrderState) {
                                case OrderState::Idle:
                                    uint8_t *bug;
                                    isOrderAccepted = m_messenger.ReceiveOrder(bug, m_hdr.MessageType);
                                    if (isOrderAccepted) {
                                        currentOrderState = OrderState::SendExecutionReport;
                                        std::cout << "SendExecutionReport" << std::endl;
                                    } else {
                                        currentOrderState = OrderState::SendCancelReject;
                                        std::cout << "SendCancelReject" << std::endl;
                                    }
                                    break;
                                case OrderState::SendExecutionReport:
                                    m_messenger.Make8((uint8_t*)buffer, m_hdr.TargetCompID, 94);
                                    session->SendPacket((uint8_t*)buffer, sizeof(FIX_8_t));
                                    printf("[SERVER] port %d, session %d, send 8 \n", m_port, session->GetSessionNum());
                                    currentOrderState = OrderState::Idle;
                                    break;
                                case OrderState::SendCancelReject:
                                    m_messenger.Make9((uint8_t*)buffer, m_hdr.TargetCompID, 94);
                                    session->SendPacket((uint8_t*)buffer, sizeof(FIX_9_t));
                                    printf("[SERVER] port %d, session %d, send 9 \n", m_port, session->GetSessionNum());
                                    currentOrderState = OrderState::Idle;
                                    break;
                                default:
                                    currentOrderState = OrderState::Idle;
                                    break;
                            }
                            currentState = State::Ordering;
                        }
                        else if ((m_hdr.MessageType == order.FIXMsgType["Heartbeat"] && !recvMsgError)) {
                            m_rcv = true;
                            currentState = State::Ordering;
                            std::cout << "Ordering" << std::endl;
                        }
                        else if (m_hdr.MessageType == order.FIXMsgType["TestRequest"] && !recvMsgError) {
                            m_rcv = true;
                            m_messenger.Make0((uint8_t*)buffer, m_hdr.TargetCompID, 94);
                            session->SendPacket((uint8_t*)buffer, sizeof(FIX_0_t));
                            currentState = State::Ordering;
                            std::cout << "Ordering" << std::endl;
                        }
                        else if (m_hdr.MessageType == order.FIXMsgType["ResendRequest"] && !recvMsgError) {
                            m_rcv = true;
                            m_messenger.Make2resp((uint8_t*)buffer, m_hdr.TargetCompID, 94);
                            session->SendPacket((uint8_t*)buffer, sizeof(FIX_2_t));
                            currentState = State::Ordering;
                            std::cout << "Ordering" << std::endl;
                        }
                        else if (m_hdr.MessageType == order.FIXMsgType["RejectSession"] && !recvMsgError) {
                            m_rcv = true;
                            m_messenger.Make3resp((uint8_t*)buffer, m_hdr.TargetCompID, 94);
                            session->SendPacket((uint8_t*)buffer, sizeof(FIX_3_t)); 
                            currentState = State::Ordering;
                            std::cout << "Ordering" << std::endl;     
                        }
                        else if (m_hdr.MessageType == order.FIXMsgType["Logout"]) {
                            m_rcv = true;
                            m_messenger.Make5((uint8_t*)buffer, m_hdr.TargetCompID, 94);
                            session->SendPacket((uint8_t*)buffer, sizeof(FIX_5_t));
                            currentState = State::Idle;
                            std::cout << "Logoutted" << std::endl;
                            return 1;
                        }
                        else if (recvMsgError == 2) {
                            m_rcv = true;
                            m_messenger.Make2((uint8_t*)buffer, &m_2_data, m_hdr.TargetCompID, 94);
                            session->SendPacket((uint8_t*)buffer, sizeof(FIX_2_t));
                            printf("[SERVER] port %d, session %d, send 2 \n", m_port, session->GetSessionNum());
                            currentState = State::Ordering;
                            std::cout << "Ordering" << std::endl;
                        }
                        else if (recvMsgError == 3) {
                            m_rcv = true;
                            m_messenger.Make3((uint8_t*)buffer, &m_3_data, m_hdr.TargetCompID, 94);
                            session->SendPacket((uint8_t*)buffer, sizeof(FIX_3_t));
                            printf("[SERVER] port %d, session %d, send 3 \n", m_port, session->GetSessionNum());
                            currentState = State::Ordering;
                            std::cout << "Ordering" << std::endl;
                        }
                        else {
                            m_messenger.Make5((uint8_t*)buffer, m_hdr.TargetCompID, 94);
                            session->SendPacket((uint8_t*)buffer, sizeof(FIX_5_t));
                            start = std::chrono::steady_clock::now();
                            currentState = State::SendingLogout;
                            std::cout << "error state order, SendingLogout" << std::endl;
                        }
                        break;
                    case State::SendingLogout:
                        if (m_hdr.MessageType == order.FIXMsgType["Logout"]) {
                            m_rcv = true;
                            start = std::chrono::steady_clock::now();
                            currentState = State::Idle;
                            std::cout << "Logoutted" << std::endl;
                            return 1;
                        } 
                        else if ((m_hdr.MessageType == order.FIXMsgType["Heartbeat"] && !recvMsgError) || recvMsgError == 3 || recvMsgError == 2) {
                            m_rcv = true;
                            currentState = State::SendingLogout;
                        }
                        else if (m_hdr.MessageType == order.FIXMsgType["TestRequest"] && !recvMsgError) {
                            m_rcv = true;
                            m_messenger.Make0((uint8_t*)buffer, m_hdr.TargetCompID, 94);
                            session->SendPacket((uint8_t*)buffer, sizeof(FIX_0_t));
                            currentState = State::SendingLogout;
                            std::cout << "Logoutted" << std::endl;
                        }
                        else {
                            auto end = std::chrono::steady_clock::now();
                            std::chrono::duration<double> elapsed = end - start;
                            if (elapsed.count() > 5) {
                                currentState = State::Idle;
                                std::cout << "Logoutted" << std::endl;
                                return 1;
                            }
                            currentState = State::SendingLogout;
                        }
                        break;
                    default:
                        m_rcv = false;
                        currentState = currentState;
                        break;
                }
            }

            printf("[SERVER] port %d, TargetCompID %s, session_idx %d, wait recv order\n", m_port, m_hdr.TargetCompID.c_str(), session->GetSessionNum());
        });
    }

    for(auto& t : m_vec_threads) 
    {
        if (t.joinable())
            t.join();
    }

    return 0;
}

int main()
{
    TaifexOrderServer server("127.0.0.1", 9000, 10);
    server.Start();
}
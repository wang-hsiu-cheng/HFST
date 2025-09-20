#include "TaifexOrderServer.h"

int Session::RecvSinglePacket(std::string& outputPacket, unsigned int flags) {
    outputPacket.clear();

    while (true) {
        // 先檢查內部 buffer 是否已經有完整封包
        size_t pos = findCompletePacket();
        cout << "pos: "<< pos << endl;
        if (pos != MAX_SIZE) {
            // cout << "t2";
            // 取出完整封包
            outputPacket.assign(m_recvBuffer.begin(), m_recvBuffer.begin() + pos);
            // 移除已處理的資料
            m_recvBuffer.erase(m_recvBuffer.begin(), m_recvBuffer.begin() + pos);
            std::cout << outputPacket << std::endl;
            return outputPacket.size();
        }

        // buffer 不夠完整，從 socket 接收新資料
        // cout << "t1";
        ssize_t recvSize = recv(m_sockfd, &m_tmpRecvBuffer, MAX_SIZE, flags);
        // std::cout << m_tmpRecvBuffer.data() << std::endl;
        if (recvSize <= 0) {
            return recvSize; // 0: closed, -1: error
        }

        // 加入內部 buffer
        m_recvBuffer.insert(m_recvBuffer.end(),
                            m_tmpRecvBuffer.begin(),
                            m_tmpRecvBuffer.begin() + recvSize);
    }
}

int Session::SendPacket(const std::string &buf)
{
    int sentSize = send(m_sockfd, buf.data(), buf.size(), 0);
    m_msgSeqNum++;
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
            std::string buffer;
            int recvSize = 0;
            int recvMsgError = 0;
            auto start = std::chrono::steady_clock::now();
            std::cout << "[Session " << session->GetSessionNum() << "] Connected from " << inet_ntoa(clientAddr.sin_addr) << std::endl;
            m_hdr.MsgSeqNum = 0;
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
                            session->SendPacket(m_messenger.Make0(buffer, m_hdr, session->GetMsgSeqNum()));
                            {
                                std::lock_guard<std::mutex> lock(m_sessionMutex);
                                m_sent_0_cnt++;
                            }
                            printf("[SERVER][heartbeat] port %d, session %d, send 0(%d)\n", m_port, session->GetSessionNum(), m_sent_0_cnt);
                        }
                        if (m_rcv) {
                            session->SendPacket(m_messenger.Make1(buffer, m_hdr, session->GetMsgSeqNum()));
                        }
                    }
                }
            });
            delayed_thread.detach();

            while (1) {
                recvSize = session->RecvSinglePacket(buffer);
                std::cout << "buffer size: " << buffer.size() << std::endl;
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
                            if (m_messenger.ReceiveA(buffer, &m_A_data)) {
                                SetHeartBeatIntervalInSec(m_A_data.HeartBtInt);
                                session->SendPacket(m_messenger.MakeA(buffer, m_hdr, session->GetMsgSeqNum()));
                                printf("[SERVER] port %d, session %d, send A \n", m_port, session->GetSessionNum());
                                isLogon = true;
                                currentState = State::Ordering;
                                std::cout << "Ordering" << std::endl;
                            } else {
                                session->SendPacket(m_messenger.Make5(buffer, m_hdr, session->GetMsgSeqNum()));
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
                            session->SendPacket(m_messenger.Make0(buffer, m_hdr, session->GetMsgSeqNum()));
                            currentState = State::Logging;
                            std::cout << "Logging" << std::endl;
                        }
                        else if (m_hdr.MessageType == order.FIXMsgType["ResendRequest"] && !recvMsgError) {
                            m_rcv = true;
                            session->SendPacket(m_messenger.Make2resp(buffer, m_hdr, session->GetMsgSeqNum()));
                            currentState = State::Logging;
                            std::cout << "Logging" << std::endl;
                        }
                        else if (m_hdr.MessageType == order.FIXMsgType["RejectSession"] && !recvMsgError) {
                            m_rcv = true;
                            session->SendPacket(m_messenger.Make3resp(buffer, m_hdr, session->GetMsgSeqNum()));
                            currentState = State::Logging;
                            std::cout << "Logging" << std::endl;      
                        }
                        else if (m_hdr.MessageType == order.FIXMsgType["Logout"]) {
                            m_rcv = true;
                            session->SendPacket(m_messenger.Make5(buffer, m_hdr, session->GetMsgSeqNum()));
                            currentState = State::Idle;
                            std::cout << "Logoutted" << std::endl;
                            return 1;
                        }
                        else if (recvMsgError == 2) {
                            m_rcv = true;
                            session->SendPacket(m_messenger.Make2(&m_2_data, m_hdr, session->GetMsgSeqNum()));
                            printf("[SERVER] port %d, session %d, send 2 \n", m_port, session->GetSessionNum());
                            currentState = State::Logging;
                            std::cout << "Logging" << std::endl;
                        }
                        else if (recvMsgError == 3) {
                            m_rcv = true;
                            session->SendPacket(m_messenger.Make3(&m_3_data, m_hdr, session->GetMsgSeqNum()));
                            printf("[SERVER] port %d, session %d, send 3 \n", m_port, session->GetSessionNum());
                            currentState = State::Logging;
                            std::cout << "Logging" << std::endl;
                        }
                        else if (recvMsgError == 5) {
                            m_rcv = true;
                            session->SendPacket(m_messenger.Make5(buffer, m_hdr, session->GetMsgSeqNum()));
                            start = std::chrono::steady_clock::now();
                            currentState = State::SendingLogout;
                            std::cout << "SendingLogout" << std::endl;
                        }
                        else {
                            session->SendPacket(m_messenger.Make5(buffer, m_hdr, session->GetMsgSeqNum()));
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
                            bool isOrderAccepted = m_messenger.ReceiveOrder(buffer, m_hdr.MessageType);
                            if (isOrderAccepted) {
                                session->SendPacket(m_messenger.Make8(buffer, m_hdr, session->GetMsgSeqNum()));
                                printf("[SERVER] port %d, session %d, send 8 \n", m_port, session->GetSessionNum());
                                std::cout << "SendExecutionReport" << std::endl;
                            } else {
                                session->SendPacket(m_messenger.Make9(buffer, m_hdr, session->GetMsgSeqNum()));
                                printf("[SERVER] port %d, session %d, send 9 \n", m_port, session->GetSessionNum());
                                std::cout << "SendCancelReject" << std::endl;
                            }
                        }
                        else if ((m_hdr.MessageType == order.FIXMsgType["Heartbeat"] && !recvMsgError)) {
                            m_rcv = true;
                            currentState = State::Ordering;
                            std::cout << "Ordering" << std::endl;
                        }
                        else if (m_hdr.MessageType == order.FIXMsgType["TestRequest"] && !recvMsgError) {
                            m_rcv = true;
                            session->SendPacket(m_messenger.Make0(buffer, m_hdr, session->GetMsgSeqNum()));
                            currentState = State::Ordering;
                            std::cout << "Ordering" << std::endl;
                        }
                        else if (m_hdr.MessageType == order.FIXMsgType["ResendRequest"] && !recvMsgError) {
                            m_rcv = true;
                            session->SendPacket(m_messenger.Make2resp(buffer, m_hdr, session->GetMsgSeqNum()));
                            currentState = State::Ordering;
                            std::cout << "Ordering" << std::endl;
                        }
                        else if (m_hdr.MessageType == order.FIXMsgType["RejectSession"] && !recvMsgError) {
                            m_rcv = true;
                            session->SendPacket(m_messenger.Make3resp(buffer, m_hdr, session->GetMsgSeqNum())); 
                            currentState = State::Ordering;
                            std::cout << "Ordering" << std::endl;     
                        }
                        else if (m_hdr.MessageType == order.FIXMsgType["Logout"]) {
                            m_rcv = true;
                            session->SendPacket(m_messenger.Make5(buffer, m_hdr, session->GetMsgSeqNum()));
                            currentState = State::Idle;
                            std::cout << "Logoutted" << std::endl;
                            return 1;
                        }
                        else if (recvMsgError == 2) {
                            m_rcv = true;
                            session->SendPacket(m_messenger.Make2(&m_2_data, m_hdr, session->GetMsgSeqNum()));
                            printf("[SERVER] port %d, session %d, send 2 \n", m_port, session->GetSessionNum());
                            currentState = State::Ordering;
                            std::cout << "Ordering" << std::endl;
                        }
                        else if (recvMsgError == 3) {
                            m_rcv = true;
                            session->SendPacket(m_messenger.Make3(&m_3_data, m_hdr, session->GetMsgSeqNum()));
                            printf("[SERVER] port %d, session %d, send 3 \n", m_port, session->GetSessionNum());
                            currentState = State::Ordering;
                            std::cout << "Ordering" << std::endl;
                        }
                        else {
                            session->SendPacket(m_messenger.Make5(buffer, m_hdr, session->GetMsgSeqNum()));
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
                            session->SendPacket(m_messenger.Make0(buffer, m_hdr, session->GetMsgSeqNum()));
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
    TaifexOrderServer server("127.0.0.1", 9000, 1);
    server.Start();
}

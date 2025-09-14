#include "TaifexOrderClient.h"

int Session::RecvSinglePacket(std::string& _outputPacket, unsigned int _flags) {
    _outputPacket.clear();

    while (true) {
        // 先檢查內部 buffer 是否已經有完整封包
        size_t pos = FindCompletePacket();
        cout << "pos: "<< pos << endl;
        if (pos != MAX_SIZE) {
            // cout << "t2";
            // 取出完整封包
            _outputPacket.assign(m_recv_buffer.begin(), m_recv_buffer.begin() + pos);
            // 移除已處理的資料
            m_recv_buffer.erase(m_recv_buffer.begin(), m_recv_buffer.begin() + pos);
            std::cout << _outputPacket << std::endl;
            return _outputPacket.size();
        }

        // buffer 不夠完整，從 socket 接收新資料
        // cout << "t1";
        ssize_t recvSize = recv(m_sockfd, m_tmp_recv_buffer.data(), MAX_SIZE, _flags);
        // std::cout << m_tmp_recv_buffer.data() << std::endl;
        if (recvSize <= 0) {
            return recvSize; // 0: closed, -1: error
        }

        // 加入內部 buffer
        m_recv_buffer.insert(m_recv_buffer.end(),
                            m_tmp_recv_buffer.begin(),
                            m_tmp_recv_buffer.begin() + recvSize);
    }
}

int Session::SendPacket(const std::string &buf)
{
    int sentSize = send(m_sockfd, buf.data(), buf.size(), 0);
    m_msg_seq_num++;
    return sentSize;
}

/************************************ TaifexOrderClient ************************************/

int TaifexOrderClient::Start()
{
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd < 0) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    sockaddr_in clientAdder{};
    clientAdder.sin_family = AF_INET;
    clientAdder.sin_port = htons(m_port);
    // Convert m_client_ip into binary form
    if (inet_pton(AF_INET, m_client_ip.c_str(), &clientAdder.sin_addr) <= 0) {
        std::cerr << "Invalid address/Address not supported\n";
        return 1;
    }
    // Connect to server
    if (connect(m_sockfd, (struct sockaddr*)&clientAdder, sizeof(clientAdder)) < 0) {
        std::cerr << "Connection failed\n";
        return 1;
    }
    std::cout << "Connected to server.\n";

    // 將連線封裝成 Session 物件
    auto session = std::make_shared<Session>(m_sockfd);
    // start login
    std::string buffer;
    int recvSize = 0;
    int recvMsgError = 0;
    auto start = std::chrono::steady_clock::now();
    m_hdr.MsgSeqNum = 0;
    current_state = State::Logging;
    current_order_state = OrderState::Idle;
    std::thread delayedThread([this, session]()
    {
        char buffer[32] = {0};
        while(1)
        {
            EnableHeartBeat();
            sleep(m_heartbeat_interval_sec * 2);
            if (m_en_hb) {
                session->SendPacket(m_messenger.Make0(buffer, m_hdr, session->GetMsgSeqNum()));
                m_sent_0_cnt++;
            }
        }
    });
    delayedThread.detach();

    SetHeartBeatIntervalInSec(10);
    session->SendPacket(m_messenger.MakeA(buffer, m_hdr, session->GetMsgSeqNum()));
    while (1) {
        if (!is_waiting_order) {
            recvSize = session->RecvSinglePacket(buffer);
            m_rcv = false;
            if (recvSize <= 0) 
            {
                std::cerr << "Session Connection error." << std::endl;
                return 0;
            }
            recvMsgError = m_messenger.SearchMsgType(buffer, m_hdr, m_2_data, m_3_data);
        }
        
        switch (current_state) {
            case State::Logging:
                if (m_hdr.MessageType == order.FIXMsgType["Logon"] && !recvMsgError) {
                    m_rcv = true;
                    is_logon = true;
                    current_state = State::Ordering;
                    is_waiting_order = true;
                    std::cout << "Ordering" << std::endl;
                }
                else if (m_hdr.MessageType == order.FIXMsgType["ResendRequest"] && !recvMsgError) {
                    m_rcv = true;
                    SetHeartBeatIntervalInSec(10);
                    session->SendPacket(m_messenger.MakeA(buffer, m_hdr, session->GetMsgSeqNum()));
                    current_state = State::Logging;
                    std::cout << "Logging" << std::endl;
                }
                else if (m_hdr.MessageType == order.FIXMsgType["RejectSession"] && !recvMsgError) {
                    m_rcv = true;
                    session->SendPacket(m_messenger.Make3resp(buffer, m_hdr, session->GetMsgSeqNum()));
                    current_state = State::Logging;
                    std::cout << "Logging" << std::endl;      
                }
                else if (m_hdr.MessageType == order.FIXMsgType["Logout"] || m_hdr.MessageType == order.FIXMsgType["Heartbeat"] || m_hdr.MessageType == order.FIXMsgType["TestRequest"]) {
                    m_rcv = true;
                    session->SendPacket(m_messenger.Make5(buffer, m_hdr, session->GetMsgSeqNum()));
                    current_state = State::Idle;
                    std::cout << "Logoutted" << std::endl;
                    return 1;
                }
                else if (recvMsgError == 2) {
                    m_rcv = true;
                    session->SendPacket(m_messenger.Make2(&m_2_data, m_hdr, session->GetMsgSeqNum()));
                    printf("[SERVER] port %d, send 2 \n", m_port);
                    current_state = State::Logging;
                    std::cout << "Logging" << std::endl;
                }
                else if (recvMsgError == 3) {
                    m_rcv = true;
                    session->SendPacket(m_messenger.Make3(&m_3_data, m_hdr, session->GetMsgSeqNum()));
                    printf("[SERVER] port %d, send 3 \n", m_port);
                    current_state = State::Logging;
                    std::cout << "Logging" << std::endl;
                }
                else if (recvMsgError == 5) {
                    m_rcv = true;
                    session->SendPacket(m_messenger.Make5(buffer, m_hdr, session->GetMsgSeqNum()));
                    start = std::chrono::steady_clock::now();
                    current_state = State::SendingLogout;
                    std::cout << "SendingLogout" << std::endl;
                }
                else {
                    SetHeartBeatIntervalInSec(10);
                    session->SendPacket(m_messenger.MakeA(buffer, m_hdr, session->GetMsgSeqNum()));
                    current_state = State::Logging;
                    std::cout << "Logging" << std::endl;
                }
                break;
            case State::Ordering:
                if (m_hdr.MessageType == order.FIXMsgType["ExecutionReport"]) {
                    m_rcv = true;
                    current_state = State::Ordering;
                    is_waiting_order = true;
                    m_hdr.MessageType = "";
                    std::cout << "Ordering" << std::endl;
                }
                else if (m_hdr.MessageType == order.FIXMsgType["OrderCancelReject"]) {
                    m_rcv = true;
                    current_state = State::Ordering;
                    is_waiting_order = true;
                    m_hdr.MessageType = "";
                    std::cout << "Ordering" << std::endl;
                }
                else if (m_hdr.MessageType == order.FIXMsgType["TestRequest"] && !recvMsgError) {
                    m_rcv = true;
                    session->SendPacket(m_messenger.Make0(buffer, m_hdr, session->GetMsgSeqNum()));
                    current_state = State::Ordering;
                    is_waiting_order = false;
                    std::cout << "Ordering" << std::endl;
                }
                else if (m_hdr.MessageType == order.FIXMsgType["ResendRequest"] && !recvMsgError) {
                    m_rcv = true;
                    session->SendPacket(m_messenger.Make2resp(buffer, m_hdr, session->GetMsgSeqNum()));
                    current_state = State::Ordering;
                    is_waiting_order = false;
                    std::cout << "Ordering" << std::endl;
                }
                else if (m_hdr.MessageType == order.FIXMsgType["RejectSession"] && !recvMsgError) {
                    m_rcv = true;
                    session->SendPacket(m_messenger.Make3resp(buffer, m_hdr, session->GetMsgSeqNum())); 
                    current_state = State::Ordering;
                    is_waiting_order = false;
                    std::cout << "Ordering" << std::endl;     
                }
                else if (m_hdr.MessageType == order.FIXMsgType["Logout"]) {
                    m_rcv = true;
                    session->SendPacket(m_messenger.Make5(buffer, m_hdr, session->GetMsgSeqNum()));
                    current_state = State::Idle;
                    is_waiting_order = false;
                    std::cout << "Logoutted" << std::endl;
                    return 1;
                }
                else if (recvMsgError == 2) {
                    m_rcv = true;
                    session->SendPacket(m_messenger.Make2(&m_2_data, m_hdr, session->GetMsgSeqNum()));
                    printf("[SERVER] port %d, send 2 \n", m_port);
                    current_state = State::Ordering;
                    is_waiting_order = false;
                    std::cout << "Ordering" << std::endl;
                }
                else if (recvMsgError == 3) {
                    m_rcv = true;
                    session->SendPacket(m_messenger.Make3(&m_3_data, m_hdr, session->GetMsgSeqNum()));
                    printf("[SERVER] port %d, send 3 \n", m_port);
                    current_state = State::Ordering;
                    is_waiting_order = false;
                    std::cout << "Ordering" << std::endl;
                }
                else {
                    m_rcv = true;
                    std::string order_msg_type;
                    std::cout << "Input Order MsgType: ";
                    std::cin >> order_msg_type;
                    is_waiting_order = false;
                    if (order_msg_type == "NewOrder") {
                        session->SendPacket(m_messenger.MakeD(buffer, m_hdr, session->GetMsgSeqNum()));
                    } else if (order_msg_type == "OrderCancel") {
                        session->SendPacket(m_messenger.MakeF(buffer, m_hdr, session->GetMsgSeqNum()));
                    } else if (order_msg_type == "OrderCancelRequest") {
                        session->SendPacket(m_messenger.MakeG(buffer, m_hdr, session->GetMsgSeqNum()));
                    } else if (order_msg_type == "OrderStatus") {
                        session->SendPacket(m_messenger.MakeH(buffer, m_hdr, session->GetMsgSeqNum()));
                    } else if (order_msg_type == "Heartbeat") {
                        session->SendPacket(m_messenger.Make0(buffer, m_hdr, session->GetMsgSeqNum()));
                        is_waiting_order = true;
                    } else if (order_msg_type == "TestRequest") {
                        session->SendPacket(m_messenger.Make1(buffer, m_hdr, session->GetMsgSeqNum()));
                    } else if (order_msg_type == "Logout") {
                        session->SendPacket(m_messenger.Make5(buffer, m_hdr, session->GetMsgSeqNum()));
                    } else {
                        is_waiting_order = true;
                    }
                    current_state = State::Ordering;
                    std::cout << "Ordering" << std::endl;
                }
                break;
            case State::SendingLogout:
                if (m_hdr.MessageType == order.FIXMsgType["Logout"]) {
                    m_rcv = true;
                    start = std::chrono::steady_clock::now();
                    current_state = State::Idle;
                    std::cout << "Logoutted" << std::endl;
                    return 1;
                } 
                else if ((m_hdr.MessageType == order.FIXMsgType["Heartbeat"] && !recvMsgError) || recvMsgError == 3 || recvMsgError == 2) {
                    m_rcv = true;
                    current_state = State::SendingLogout;
                }
                else if (m_hdr.MessageType == order.FIXMsgType["TestRequest"] && !recvMsgError) {
                    m_rcv = true;
                    session->SendPacket(m_messenger.Make0(buffer, m_hdr, session->GetMsgSeqNum()));
                    current_state = State::SendingLogout;
                    std::cout << "Logoutted" << std::endl;
                }
                else {
                    auto end = std::chrono::steady_clock::now();
                    std::chrono::duration<double> elapsed = end - start;
                    if (elapsed.count() > 5) {
                        current_state = State::Idle;
                        std::cout << "Logoutted" << std::endl;
                        return 1;
                    }
                    current_state = State::SendingLogout;
                }
                break;
            default:
                m_rcv = false;
                current_state = current_state;
                break;
        }
    }
    printf("[SERVER] port %d, TargetCompID %s wait recv order\n", m_port, m_hdr.TargetCompID.c_str());
    return 0;
}

int main()
{
    TaifexOrderClient client("127.0.0.1", 9000);
    client.Start();
}
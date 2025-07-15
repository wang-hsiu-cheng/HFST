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
            if(m_recvBufferLeftSize >= (int)sizeof(TMPhdr_t))
            {
                TMPhdr_t* hdr = reinterpret_cast<TMPhdr_t*>(m_cur_recvBufferPtr);
                m_currentPacketSize = TMPGET_UINT16(hdr->msg_length) + 3;
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

void TaifexOrderServer::Start()
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
            char buffer[1500] = {0};
            int recvSize = 0;
            std::cout << "[Session " << session->GetSessionNum() << "] Connected from " << inet_ntoa(clientAddr.sin_addr) << std::endl;

            //Recv L10
            recvSize = session->RecvSinglePacket(buffer);
            if (recvSize <= 0) 
            {
                std::cerr << "[Session " << session->GetSessionNum() << "] Connection error." << std::endl;
                return;
            }
            TMPhdr_t* hdr = reinterpret_cast<TMPhdr_t*>(buffer);
            uint16_t fcm_id = TMPGET_UINT16(hdr->fcm_id);
            uint16_t session_id = TMPGET_UINT16(hdr->session_id);
            if(m_messenger.IsRecvL10((uint8_t*)buffer) != 0)
            {
                printf("[SERVER] port %d, session %d, recv L10 error\n", m_port, session->GetSessionNum());
                m_messenger.MakeL10((uint8_t*)buffer, fcm_id, session_id, 94);
                session->SendPacket((uint8_t*)buffer, sizeof(TMP_L10_t));
                return;
            }
            printf("[SERVER] port %d, session %d, recv L10 \n", m_port, session->GetSessionNum());
            
            //Send L10
            m_messenger.MakeL10((uint8_t*)buffer, fcm_id, session_id, 0);
            session->SendPacket((uint8_t*)buffer, sizeof(TMP_L10_t));
            printf("[SERVER] port %d, session %d, send L10 \n", m_port, session->GetSessionNum());

            //Recv L20
            recvSize = session->RecvSinglePacket(buffer);
            if (recvSize <= 0) 
            {
                std::cerr << "[Session " << session->GetSessionNum() << "] Connection error." << std::endl;
                return;
            }
            if(m_messenger.IsRecvL20((uint8_t*)buffer) != 0)
            {
                printf("[SERVER] port %d, session %d, recv L20 error\n", m_port, session->GetSessionNum());
                m_messenger.MakeL10((uint8_t*)buffer, fcm_id, session_id, 94);
                session->SendPacket((uint8_t*)buffer, sizeof(TMP_L10_t));
                return;
            }
            printf("[SERVER] port %d, session %d, recv L20 \n", m_port, session->GetSessionNum());
            //Send L30
            m_messenger.MakeL30((uint8_t*)buffer, fcm_id, session_id, 0, 0);
            session->SendPacket((uint8_t*)buffer, sizeof(TMP_L30_t));
            printf("[SERVER] port %d, session %d, send L30 \n", m_port, session->GetSessionNum());

            //Recv L40
            recvSize = session->RecvSinglePacket(buffer);
            if (recvSize <= 0) 
            {
                std::cerr << "[Session " << session->GetSessionNum() << "] Connection error." << std::endl;
                return;
            }
            if(m_messenger.IsRecvL40((uint8_t*)buffer) != 0)
            {
                printf("[SERVER] port %d, session %d, recv L40 error\n", m_port, session->GetSessionNum());
                m_messenger.MakeL10((uint8_t*)buffer, fcm_id, session_id, 94);
                session->SendPacket((uint8_t*)buffer, sizeof(TMP_L10_t));
                return;
            }
            printf("[SERVER] port %d, session %d, recv L40 \n", m_port, session->GetSessionNum());

            //Send L50
            m_messenger.MakeL50((uint8_t*)buffer, fcm_id, session_id, 0, 30, 160);
            session->SendPacket((uint8_t*)buffer, sizeof(TMP_L50_t));

            //Recv L60
            recvSize = session->RecvSinglePacket(buffer);
            if (recvSize <= 0) 
            {
                std::cerr << "[Session " << session->GetSessionNum() << "] Connection error." << std::endl;
                return;
            }
            if(m_messenger.IsRecvL60((uint8_t*)buffer) != 0)
            {
                printf("[SERVER] port %d, session %d, recv L60 error\n", m_port, session->GetSessionNum());
                m_messenger.MakeL10((uint8_t*)buffer, fcm_id, session_id, 94);
                session->SendPacket((uint8_t*)buffer, sizeof(TMP_L10_t));
                return;
            }
            printf("[SERVER] port %d, session %d, recv L60 \n", m_port, session->GetSessionNum());

            m_map_idx_fcmid[session->GetSessionNum()] = fcm_id;
            m_map_idx_session_id[session->GetSessionNum()] = session_id;
            m_map_session_id_sockkfd[session_id] = session->GetSockfd();

            printf("[SERVER] port %d, session_id %d, session_idx %d, login finish\n", m_port, session_id, session->GetSessionNum());

            //create thread for heartbeat
            if(m_en_hb)
            {
                std::thread hb_thread([this, session]()
                {
                    char buffer[32] = {0};
                    while(1)
                    {
                        sleep(m_heartbeat_interval_sec);
                        
                        m_messenger.MakeR04((uint8_t*)buffer, m_map_idx_fcmid[session->GetSessionNum()], m_map_idx_session_id[session->GetSessionNum()], 0);
                        session->SendPacket((uint8_t*)buffer, sizeof(TMP_R04_t));
                        {
                            std::lock_guard<std::mutex> lock(m_sessionMutex);
                            m_sent_r04_cnt++;
                        }
                        printf("[SERVER][heartbeat] port %d, session %d, send R04(%d)\n", m_port, session->GetSessionNum(), m_sent_r04_cnt);
                    }
                });
                hb_thread.detach();
            }

            printf("[SERVER] port %d, session_id %d, session_idx %d, wait recv order\n", m_port, session_id, session->GetSessionNum());
            //wait recv order
            while(1)
            {
                recvSize = session->RecvSinglePacket(buffer);
                if (recvSize <= 0) 
                {
                    std::cerr << "[Session " << session->GetSessionNum() << "] Connection error." << std::endl;
                    break;
                }

                TMPhdr_t* hdr = reinterpret_cast<TMPhdr_t*>(buffer);
                uint8_t type = TMPGET_UINT8(hdr->MessageType);
                if(type == TMPMsgType_R05)
                {
                    if(m_messenger.IsRecvR05((uint8_t*)buffer) != 0)
                    {
                        printf("[SERVER] port %d, session %d, recv R05 error\n", m_port, session->GetSessionNum());
                        break;
                    }
                    std::lock_guard<std::mutex> lock(m_sessionMutex);
                    m_recv_r05_cnt++;
                    printf("### [SERVER][%s] port %d, session %d, recv R05(%d)\n", m_messenger.util.GetTimeStr(TMPGET_UINT32(hdr->msg_time.epoch_s), TMPGET_UINT16(hdr->msg_time.ms)), m_port, session->GetSessionNum(), m_recv_r05_cnt);
                }
                else if(type == TMPMsgType_R01)
                {
                    std::lock_guard<std::mutex> lock(m_sessionMutex);
                    m_recv_r01_cnt++;
                    m_vec_r01.push_back(*reinterpret_cast<TMP_R01_t*>(buffer));

                    int r02_start = m_vec_r02.size(), r03_start = m_vec_r03.size();
                    m_repor_generator.GenReport(reinterpret_cast<TMP_R01_t*>(buffer), m_vec_r02, m_vec_r03);
                    for(int i = r02_start; i < (int)m_vec_r02.size(); i++)
                    {
                        session->SendPacket((uint8_t*)&m_vec_r02[i], sizeof(TMP_R02_t));
                        usleep(m_report_delay_us);
                        m_sent_r02_cnt++;
                    }

                    for(int i = r03_start; i < (int)m_vec_r03.size(); i++)
                    {
                        session->SendPacket((uint8_t*)&m_vec_r03[i], sizeof(TMP_R03_t));
                        usleep(m_report_delay_us);
                        m_sent_r03_cnt++;
                    }

                    printf("### [SERVER] port %d, session_id %d, recv R01 (%d), sent R02 (%d), R03 (%d)\n", m_port, m_map_idx_session_id[session->GetSessionNum()], m_recv_r01_cnt, m_sent_r02_cnt, m_sent_r03_cnt);
                }
                else if(type == TMPMsgType_R09)
                {
                    std::lock_guard<std::mutex> lock(m_sessionMutex);
                    m_recv_r09_cnt++;
                    m_vec_r09.push_back(*reinterpret_cast<TMP_R09_t*>(buffer));

                    int r02_start = m_vec_r02.size(), r03_start = m_vec_r03.size();
                    m_repor_generator.GenReport(reinterpret_cast<TMP_R09_t*>(buffer), m_vec_r02, m_vec_r03);
                    for(int i = r02_start; i < (int)m_vec_r02.size(); i++)
                    {
                        session->SendPacket((uint8_t*)&m_vec_r02[i], sizeof(TMP_R02_t));
                        usleep(m_report_delay_us);
                        m_sent_r02_cnt++;
                    }

                    for(int i = r03_start; i < (int)m_vec_r03.size(); i++)
                    {
                        session->SendPacket((uint8_t*)&m_vec_r03[i], sizeof(TMP_R03_t));
                        usleep(m_report_delay_us);
                        m_sent_r03_cnt++;
                    }

                    printf("### [SERVER] port %d, session_id %d, recv R09 (%d), sent R02 (%d), R03 (%d)\n", m_port, m_map_idx_session_id[session->GetSessionNum()], m_recv_r09_cnt, m_sent_r02_cnt, m_sent_r03_cnt);
                }
                else
                {
                    printf("[SERVER] port %d, session %d, recv unknown type %d, ", m_port, session->GetSessionNum(), type);
                    for(int i = 0; i < (int)sizeof(TMPhdr_t); i++)
                    {
                        printf("%02x", (uint8_t)buffer[i]);
                    }
                    printf("\n");
                }
            }
        });
    }

    for(auto& t : m_vec_threads) 
    {
        if (t.joinable())
            t.join();
    }
}

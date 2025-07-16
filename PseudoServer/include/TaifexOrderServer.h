#ifndef TAIFEX_ORDER_SERVER_H
#define TAIFEX_ORDER_SERVER_H

#include <iostream>
#include <vector>
#include <memory>
#include <cstring>
#include <ctime>
#include <cerrno>
#include <iomanip>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <mutex>
#include <thread>
#include <chrono>
#include <map>

#include "TaifexOrder.h"
#include "TaifexOrderUtility.h"
#include "TaifexORGenerator.h"
#include "ServerMessenger.h"

class Session
{
public:
	Session(int sockfd, int sessionNum, int listenPort)
		: m_sockfd(sockfd),
		m_sessionNum(sessionNum),
		m_listenPort(listenPort),
		m_msgSeqNum(1),
		m_recvBufferLeftSize(0)
		{
			m_recvBuffer.resize(MAX_SIZE);
			m_cur_recvBufferPtr = m_recvBuffer.data();
		}

		~Session()
		{
			if(m_sockfd >= 0)
				close(m_sockfd);
		}

	int GetSockfd() const { return m_sockfd; }
	int GetSessionNum() const { return m_sessionNum; }
	
	int RecvSinglePacket(char* outputBuffer, unsigned int flags = 0);

    int SendPacket(uint8_t *buf, int len);

private:
	int m_sockfd;
	int m_sessionNum;
	int m_listenPort;
	int m_msgSeqNum;
	
	unsigned char* m_cur_recvBufferPtr;
	
	int m_currentPacketSize = 0;
    
    std::vector<unsigned char> m_recvBuffer;
    int m_recvBufferLeftSize = 0;
};

class TaifexOrderServer
{
public:
    TaifexOrderServer(std::string server_ip, int port, int expectedSessions)
    : m_port(port), m_expectedSessions(expectedSessions)
    {
        m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (m_sockfd < 0) 
        {
            std::cerr << "Failed to create server socket." << std::endl;
            std::exit(EXIT_FAILURE);
        }

        m_server_ip = server_ip;
    }

    void SetUSDelayForReport(int us)
    {
        m_report_delay_us = us;
    }

    ~TaifexOrderServer()
    {
        if (m_sockfd >= 0)
            close(m_sockfd);
    }

    void Start();

    void SetHeartBeatIntervalInSec(int sec)
    {
        m_heartbeat_interval_sec = sec;
    }

    void EnableHeartBeat()
    {
        m_en_hb = true;
    }

    void DisableHeartBeat()
    {
        m_en_hb = false;
    }

    int GetRecvR01Cnt() const { return m_recv_r01_cnt; }
    int GetRecvR09Cnt() const { return m_recv_r09_cnt; }
    int GetRecvR05Cnt() const { return m_recv_r05_cnt; }
    int GetSentR02Cnt() const { return m_sent_r02_cnt; }
    int GetSentR03Cnt() const { return m_sent_r03_cnt; }
    int GetSent0Cnt() const { return m_sent_0_cnt; }

    std::vector<FIX_R01_t>* GetR01Vec() { return &m_vec_r01; }
    std::vector<FIX_R09_t>* GetR09Vec() { return &m_vec_r09; }
    std::vector<FIX_R02_t>* GetR02Vec() { return &m_vec_r02; }
    std::vector<FIX_R03_t>* GetR03Vec() { return &m_vec_r03; }

private:
    int m_sockfd;
    int m_port;
    int m_expectedSessions;
    std::string m_server_ip;
    bool isLogon = false;

    std::vector<std::shared_ptr<Session>> m_vec_sessions;
    std::vector<std::thread> m_vec_threads;
    std::mutex m_sessionMutex;

    ServerMessenger m_messenger;
    TaifexORGenerator m_repor_generator;

    std::map<int, uint16_t> m_map_idx_target_id;
    std::map<int, uint16_t> m_map_idx_sender_id;
    std::map<uint16_t, int> m_map_session_id_sockkfd;

    bool m_en_hb = false;

    int m_recv_r01_cnt = 0;
    int m_recv_r09_cnt = 0;
    int m_recv_r05_cnt = 0;

    int m_sent_r02_cnt = 0;
    int m_sent_r03_cnt = 0;
    int m_sent_0_cnt = 0;

    int m_report_delay_us = 5;

    int m_heartbeat_interval_sec = 30;

    std::vector<FIX_R01_t> m_vec_r01;
    std::vector<FIX_R09_t> m_vec_r09;

    std::vector<FIX_R02_t> m_vec_r02;
    std::vector<FIX_R03_t> m_vec_r03;
};

#endif
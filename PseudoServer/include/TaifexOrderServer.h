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

using namespace std;

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
			m_recvBuffer.resize(0);
            m_tmpRecvBuffer.resize(MAX_SIZE);
			m_cur_recvBufferPtr = m_recvBuffer;
		}

		~Session()
		{
			if(m_sockfd >= 0)
				close(m_sockfd);
		}

	int GetSockfd() const { return m_sockfd; }
    inline uint16_t GetMsgSeqNum() const { return m_msgSeqNum; }
	int GetSessionNum() const { return m_sessionNum; }
	
	int RecvSinglePacket(std::string& outputBuffer, unsigned int flags = 0);

    int SendPacket(const std::string &buf);

private:
	int m_sockfd;
	int m_sessionNum;
	int m_listenPort;
	uint16_t m_msgSeqNum;
	
	std::string m_cur_recvBufferPtr;
	
	int m_currentPacketSize = 0;
    
    std::string m_recvBuffer;
    std::string m_tmpRecvBuffer;
    int m_recvBufferLeftSize = 0;
    // 用於 search 查找 "10="
    const std::string fixChecksumPattern = "10=";

    size_t findCompletePacket() {
        size_t pos = m_recvBuffer.find(fixChecksumPattern);
        if (pos != std::string::npos) {
            // confirm we have at least "10=NNN␁" (7 bytes)
            if (pos + 7 <= m_recvBuffer.size()) {
                return pos + 7;  // return packet length
            }
        }
        return MAX_SIZE; // not found or incomplete
    }
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

    int Start();

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

    int GetRecvDCnt() const { return m_recv_D_cnt; }
    int GetRecvACnt() const { return m_recv_A_cnt; }
    int GetSent8Cnt() const { return m_sent_8_cnt; }
    int GetSent9Cnt() const { return m_sent_9_cnt; }
    int GetSent0Cnt() const { return m_sent_0_cnt; }

    std::vector<FIX_D_t>* GetDVec() { return &m_vec_D; }
    std::vector<FIX_8_t>* Get8Vec() { return &m_vec_8; }
    std::vector<FIX_9_t>* Get9Vec() { return &m_vec_9; }

private:
    int m_sockfd;
    int m_port;
    int m_expectedSessions;
    uint16_t serverSeqNum = 0;
    std::string m_server_ip;
    std::string ClOrdID;
    FIXhdr_t m_hdr;
    FIX_A_t m_A_data;
    FIX_2_t m_2_data;
    FIX_3_t m_3_data;
    State currentState;
    OrderState currentOrderState;

    std::vector<std::shared_ptr<Session>> m_vec_sessions;
    std::vector<std::thread> m_vec_threads;
    std::mutex m_sessionMutex;

    std::map<int, uint16_t> m_map_idx_target_id;
    std::map<int, uint16_t> m_map_idx_sender_id;
    std::map<uint16_t, int> m_map_session_id_sockkfd;

    bool m_en_hb = false;
    bool m_rcv = false;
    bool isLogon = false;

    int m_recv_D_cnt = 0;
    int m_recv_A_cnt = 0;

    int m_sent_8_cnt = 0;
    int m_sent_9_cnt = 0;
    int m_sent_0_cnt = 0;

    int m_report_delay_us = 5;

    int m_heartbeat_interval_sec = 30;

    std::vector<FIX_D_t> m_vec_D;

    std::vector<FIX_8_t> m_vec_8;
    std::vector<FIX_9_t> m_vec_9;

    ServerMessenger m_messenger;
    TaifexOrder order;
};

#endif
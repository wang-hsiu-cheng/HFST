#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <TaifexOrder.h>

class Client
{
public:
    Client(){};
    ~Client(){};
    std::string FIXComputeCheckSum(const std::string& data);
    int MakeA(uint8_t *buf, std::string TargetCompID, uint8_t status_code);
    int SendPacket(uint8_t *buf, int len);
    int Start();
private:
    int m_sockfd;
    int m_port;
    int m_expectedSessions;
    std::string m_server_ip;
    FIXhdr_t m_hdr;

    int TargetCompID = 1000;
    int session_id = 1;
};
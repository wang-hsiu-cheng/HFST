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
    void TMPHdrSet(TMPhdr_t* hdr, uint16_t msgType, uint16_t msgLen);
    void TMPSetCheckSum(uint8_t *checksum, const void *data, size_t size);
    int MakeL10(uint8_t *buf, uint16_t fcm_id, uint16_t session_id, uint8_t status_code);
    int SendPacket(uint8_t *buf, int len);
    int Start();
private:
    int m_sockfd;
    int m_port;
    int m_expectedSessions;
    std::string m_server_ip;

    int fcm_id = 1000;
    int session_id = 1;
};
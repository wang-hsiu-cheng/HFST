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

void Client::TMPHdrSet(TMPhdr_t* hdr, uint16_t msgType, uint16_t msgLen)
{
    TMPSET_MSGLEN(hdr->msg_length, msgLen);
	TMPSET_UINT8(hdr->MessageType, msgType);
	TMPSET_INT32(hdr->msg_time.epoch_s, time(NULL));
}

void Client::TMPSetCheckSum(uint8_t *checksum, const void *data, size_t size)
{
    char *ptr = (char *)data;
	char *End = ptr + size - 1;
	unsigned int Sum = 0;

	for (; ptr != End; (Sum += (unsigned char)(*(ptr++))));

	(*checksum) = (uint8_t)(Sum & 255);

	return;
}

int Client::MakeL10(uint8_t *buf, uint16_t fcm_id, uint16_t session_id, uint8_t status_code)
{
    TMP_L10_t *p = (TMP_L10_t *)buf;
    TMPHdrSet(&p->MsgHeader, TMPMsgType_L10, sizeof(TMP_L10_t));
    TMPSET_UINT16(p->MsgHeader.fcm_id, fcm_id);
    TMPSET_UINT16(p->MsgHeader.session_id, session_id);
    p->status_code = status_code;
    p->start_in_bound_num = 0;
    TMPSetCheckSum(&p->CheckSum, (const void *)p, sizeof(TMP_L10_t));

    return sizeof(TMP_L10_t);
}

int Client::SendPacket(uint8_t *buf, int len)
{
    int sentSize = send(m_sockfd, buf, len, 0);
    return sentSize;
}

int Client::Start()
{
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd < 0) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9000);
    std::cout << "test\n";
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    if (connect(m_sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed\n";
        return 1;
    }
    char *buffer;
    MakeL10((uint8_t*)buffer, fcm_id, session_id, 0);
    SendPacket((uint8_t*)buffer, sizeof(TMP_L10_t));

    close(m_sockfd);

    return 0;
}

int main() {
    Client client;
    client.Start();

    return 0;
}

#include <TestClient.h>

void Client::FIXHdrSet(FIXhdr_t* hdr, uint16_t msgType, uint16_t msgLen)
{
    FIXSET_MSGLEN(hdr->msg_length, msgLen);
	FIXSET_UINT8(hdr->MessageType, msgType);
	FIXSET_INT32(hdr->SendingTime.epoch_s, time(NULL));
}

void Client::FIXSetCheckSum(uint8_t *checksum, const void *data, size_t size)
{
    char *ptr = (char *)data;
	char *End = ptr + size - 1;
	unsigned int Sum = 0;

	for (; ptr != End; (Sum += (unsigned char)(*(ptr++))));

	(*checksum) = (uint8_t)(Sum & 255);

	return;
}

int Client::MakeL10(uint8_t *buf, uint16_t TargetCompID, uint16_t session_id, uint8_t status_code)
{
    FIX_L10_t *p = (FIX_L10_t *)buf;
    FIXHdrSet(&p->MsgHeader, FIXMsgType_L10, sizeof(FIX_L10_t));
    FIXSET_UINT16(p->MsgHeader.TargetCompID, TargetCompID);
    FIXSET_UINT16(p->MsgHeader.session_id, session_id);
    p->status_code = status_code;
    p->start_in_bound_num = 0;
    FIXSetCheckSum(&p->CheckSum, (const void *)p, sizeof(FIX_L10_t));

    return sizeof(FIX_L10_t);
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
    MakeL10((uint8_t*)buffer, TargetCompID, session_id, 0);
    SendPacket((uint8_t*)buffer, sizeof(FIX_L10_t));

    close(m_sockfd);

    return 0;
}

int main() {
    Client client;
    client.Start();

    return 0;
}

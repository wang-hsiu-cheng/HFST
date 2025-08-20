#include <TestClient.h>

std::string Client::FIXComputeCheckSum(const std::string& data)
{
    const char* ptr = data.data();
    const char* End = ptr + data.size() - 1;  // exclude last byte (SOH after tag 10)
    unsigned int sum = 0;

    for (; ptr != End; ++ptr) {
        sum += static_cast<unsigned char>(*ptr);
    }

    uint8_t checksumValue = static_cast<uint8_t>(sum & 0xFF);
    std::ostringstream oss;
    oss << std::setw(3) << std::setfill('0') << static_cast<int>(checksumValue);
    return oss.str();  // e.g., "157", "003"
}

int Client::MakeA(uint8_t *buf, std::string TargetCompID, uint8_t status_code)
{
    std::string buf_str(reinterpret_cast<char*>(buf));
    // util.FIXHdrSet(&p->MsgHeader, FIXMsgType_A, sizeof(FIX_A_t));
    // FIXSET_UINT16(p->MsgHeader.TargetCompID, TargetCompID);
    // FIXSET_UINT16(p->MsgHeader.session_id, session_id);
    // p->status_code = status_code;
    // p->start_in_bound_num = 0;
    FIXComputeCheckSum(buf_str);

    return sizeof(FIX_A_t);
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
    MakeA((uint8_t*)buffer, m_hdr.TargetCompID, 94);
    SendPacket((uint8_t*)buffer, sizeof(FIX_A_t));

    close(m_sockfd);

    return 0;
}

int main() {
    Client client;
    client.Start();

    return 0;
}

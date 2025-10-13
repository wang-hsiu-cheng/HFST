#include <pcap.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <iomanip>
#include <net/if.h>
#include <netpacket/packet.h>
#include <netinet/ip.h>     // struct ip
#include <netinet/tcp.h>    // struct tcphdr
#include <netinet/if_ether.h> // struct ether_header

using namespace std;

struct pseudo_header {
    uint32_t src_addr;   // IP source
    uint32_t dst_addr;   // IP dest
    uint8_t  zero;       // must be 0
    uint8_t  protocol;   // TCP = 6
    uint16_t tcp_len;    // TCP header + payload length
} psh;

// #define TH_FIN  0x01
// #define TH_SYN  0x02
// #define TH_RST  0x04
// #define TH_PUSH 0x08
// #define TH_ACK  0x16
// #define TH_URG  0x32

// struct ip {
// #if __BYTE_ORDER == __LITTLE_ENDIAN
//     unsigned int ip_hl:4;   /* header length */
//     unsigned int ip_v:4;    /* version */
// #elif __BYTE_ORDER == __BIG_ENDIAN
//     unsigned int ip_v:4;    /* version */
//     unsigned int ip_hl:4;   /* header length */
// #endif
//     u_int8_t  ip_tos;       /* type of service */
//     u_short   ip_len;       /* total length */
//     u_short   ip_id;        /* identification */
//     u_short   ip_off;       /* fragment offset field */
//     u_int8_t  ip_ttl;       /* time to live */
//     u_int8_t  ip_p;         /* protocol */
//     u_short   ip_sum;       /* checksum */
//     struct    in_addr ip_src, ip_dst; /* source and dest address */
// };

// struct tcphdr {
//     u_short th_sport;   /* source port */
//     u_short th_dport;   /* destination port */
//     tcp_seq th_seq;     /* sequence number */
//     tcp_seq th_ack;     /* acknowledgement number */
// #if __BYTE_ORDER == __LITTLE_ENDIAN
//     u_int   th_x2:4;    /* (unused) */
//     u_int   th_off:4;   /* data offset */
// #elif __BYTE_ORDER == __BIG_ENDIAN
//     u_int   th_off:4;   /* data offset */
//     u_int   th_x2:4;    /* (unused) */
// #endif
//     u_char  th_flags;   /* control flags (SYN, ACK, FIN, etc.) */
//     u_short th_win;     /* window */
//     u_short th_sum;     /* checksum */
//     u_short th_urp;     /* urgent pointer */
// };

struct in_addr server_address;
u_short server_port;
struct ether_header *eth;
struct ip *iph;
int ip_header_len;
struct in_addr ip_source_address;
struct tcphdr *tcph;
string app_msg;
int tcp_header_len;
bool rcv_packet_loss = false;
bool receive_msg = false;
bool msg_sent = false;

unsigned char ip_packet[1500];
unsigned char tcp_packet[1500];
struct ip *send_iph = (struct ip*)ip_packet;
int send_ip_header_len;
struct in_addr send_ip_source_address;
int id_counter = 0;
struct tcphdr *send_tcph = (struct tcphdr*)tcp_packet;
string send_app_msg;
int send_app_len;
int send_tcp_header_len;

unsigned short csum(unsigned short *ptr, int nbytes) {
    unsigned long sum = 0;
    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }
    if (nbytes == 1) {
        sum += *(unsigned char*)ptr;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

void ether_sender(std::string _payload, struct tcphdr *_send_tcph, struct ip *_send_iph)
{
    int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock < 0) { perror("socket"); exit(1); }

    // Example MAC addresses (replace with real ones)
    unsigned char dst_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    unsigned char src_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    // Build Ethernet header
    unsigned char frame[1514];
    memcpy(frame, dst_mac, 6);
    memcpy(frame+6, src_mac, 6);
    frame[12] = 0x08; frame[13] = 0x00; // IPv4 EtherType

    int ip_header_len  = _send_iph->ip_hl * 4;
    int tcp_header_len = _send_tcph->th_off * 4;

    // Frame total length = 14 (Ethernet) + ip_total_len
    // cout << "ipl: " << ip_header_len << " tcpl: " << tcp_header_len << " payloadl: " << _payload.size() << std::endl;
    _send_iph->ip_len = htons(ip_header_len + tcp_header_len + _payload.size());
    _send_iph->ip_sum = 0;       /* checksum */
    _send_iph->ip_sum = csum((unsigned short*)_send_iph, ip_header_len);
    // cout << "total: " << ntohs(_send_iph->ip_len);
    int frame_len = 14 + ntohs(_send_iph->ip_len);

    // Copy IP header + TCP header + payload into Ethernet frame after 14 bytes
    memcpy(frame + 14, _send_iph, ip_header_len);
    memcpy(frame + 14 + ip_header_len, _send_tcph, tcp_header_len);
    memcpy(frame + 14 + ip_header_len + tcp_header_len,
        _payload.data(), _payload.size());


    // Interface info
    struct sockaddr_ll device;
    memset(&device, 0, sizeof(device));
    device.sll_ifindex = if_nametoindex("lo"); // replace with your iface
    device.sll_halen = ETH_ALEN;
    memcpy(device.sll_addr, dst_mac, 6);
    
    // cout << "source tcp port: " << ntohs(_send_tcph->th_sport) << std::endl;
    // cout << "dest tcp port: " << ntohs(_send_tcph->th_dport) << std::endl;
    // cout << "frame len: " << frame_len << std::endl;

    // Send
    if (sendto(sock, frame, frame_len, 0,
              (struct sockaddr*)&device, sizeof(device)) < 0) {
        perror("sendto");
        exit(1);
    }

    close(sock);
}

void ip_encoder(int payload_len)
{
    send_iph->ip_hl = 5;
    send_iph->ip_v = 4;    /* version */
    send_iph->ip_tos = 0;       /* type of service */
    send_iph->ip_id = htons(id_counter++);        /* identification */
    send_iph->ip_off = 0;       /* fragment offset field */
    send_iph->ip_ttl = 64;       /* time to live */
    send_iph->ip_p = IPPROTO_TCP;         /* protocol */
    send_iph->ip_src.s_addr = inet_addr("127.0.0.1");
    send_iph->ip_dst.s_addr = inet_addr("127.0.0.1");
}

void tcp_encoder(string payload, int FLAG)
{
    if (FLAG == TH_SYN) {
        uint32_t isn = rand();
        send_tcph->th_seq = htonl(isn);
        send_tcph->th_ack = 0;
        send_tcph->th_sport = htons(rand() % 50000 + 1024);
        send_tcph->th_dport = htons(9000);
    } else if (FLAG == TH_ACK || FLAG == TH_PUSH || FLAG == TH_PUSH+TH_ACK) {
        send_tcph->th_seq = tcph->th_ack;
        send_tcph->th_ack = tcph->th_seq + 1;
        send_tcph->th_sport = tcph->th_dport;
        send_tcph->th_dport = tcph->th_sport;
    }
    send_tcph->th_flags = FLAG;
    send_tcph->th_off = 5;
    send_tcph->th_win = 65495;
    send_tcph->th_urp = 0;

    psh.src_addr = send_iph->ip_src.s_addr;
    psh.dst_addr = send_iph->ip_dst.s_addr;
    psh.zero     = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_len  = htons(send_tcph->th_off * 4 + payload.size());
    // cout << "tcp_len: " << send_tcph->th_off * 4 + payload.size();

    // calculate total length
    int psize = sizeof(psh) + psh.tcp_len;
    unsigned char *pseudogram = (unsigned char*)malloc(psize);

    // copy pseudo-header
    memcpy(pseudogram, &psh, sizeof(psh));
    // copy TCP header
    memcpy(pseudogram + sizeof(psh), send_tcph, send_tcph->th_off * 4);
    // copy payload
    memcpy(pseudogram + sizeof(psh) + send_tcph->th_off * 4,
        payload.data(), payload.size());

    // set checksum field to 0 before computing
    send_tcph->check = 0;
    send_tcph->check = csum((unsigned short*)pseudogram, psize);

    free(pseudogram);

    ip_encoder(payload.size());
    ether_sender(payload, send_tcph, send_iph);
}

// 封包處理回呼函式
void packet_handler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    // Print in hex
    for (u_int i = 0; i < header->len; i++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << (unsigned int)packet[i] << " ";
        if ((i + 1) % 16 == 0) std::cout << std::endl;
    }
    std::cout << std::dec << std::endl << "--------------------------------" << std::endl;

    // 解析 Ethernet Header
    eth = (struct ether_header*) packet;
    if (ntohs(eth->ether_type) != ETHERTYPE_IP) {
        return; // 非 IP 封包
    }

    // 解析 IP Header
    iph = (struct ip*)(packet + sizeof(struct ether_header));
    if (iph->ip_p != IPPROTO_TCP) {
        return; // 非 TCP 封包
    }
    // IP header 長度 (iph->ip_hl 是 32-bit word count，所以要 *4 才是 bytes)
    ip_header_len = iph->ip_hl * 4;
    ip_source_address = iph->ip_src;
    if (ip_source_address.s_addr != server_address.s_addr) {
        return;
    }
    
    // 解析 TCP Header
    tcph = (struct tcphdr*)((u_char*)iph + ip_header_len);
    // unsigned char *ptr = (unsigned char*)iph + ip_header_len;
    // for (int i = 0; i < 20; i++) {
    //     printf("%02x ", ptr[i]);
    // }
    if (tcph->th_sport != server_port) {
        // cout << "error port!" << std::endl;
        return;
    }
    std::cout << "Packet captured, length: " << header->len << " bytes" << std::endl;
    printf("Source MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
            eth->ether_shost[0],
            eth->ether_shost[1],
            eth->ether_shost[2],
            eth->ether_shost[3],
            eth->ether_shost[4],
            eth->ether_shost[5]);
    cout << "Source IP: " << inet_ntoa(iph->ip_src) << std::endl;
    cout << "Server IP: " << inet_ntoa(server_address) << std::endl;
    cout << "Captured TCP Packet: "
         << inet_ntoa(iph->ip_src) << ":" << ntohs(tcph->th_sport)
         << " -> "
         << inet_ntoa(iph->ip_dst) << ":" << ntohs(tcph->th_dport)
         << endl;

    tcp_header_len = tcph->doff * 4;
    char *payload = (char *)((u_char*)tcph + tcp_header_len);
    int payload_len = ntohs(iph->ip_len) - (ip_header_len + tcp_header_len);
    std::string pl(payload, payload_len);
    app_msg = pl;

    receive_msg = true;
    msg_sent = false;
}

void main_function()
{
    bool established = false;
    cout << 1 << std::endl;
    while (1) {
        // cout << 0;
        if (receive_msg && established) {
            receive_msg = false;
            // process(fix);
            // sleep(3);
            cout << "<<PUSH>>" << std::endl;
            if (msg_sent)
                tcp_encoder(app_msg, TH_PUSH);
            else
                tcp_encoder(app_msg, TH_PUSH+TH_ACK);
            msg_sent = true;
        } else if (receive_msg && !established) {
            receive_msg = false;
            cout << "<<ACK>>" << std::endl;
            tcp_encoder("", TH_ACK);
            established = true;
            msg_sent = true;
        } else if (!receive_msg && !established) {
            cout << "<<SYN>>" << std::endl;
            tcp_encoder("01234567890123456789", TH_SYN);
        }
        this_thread::sleep_for(chrono::milliseconds(200));
    }
}

void ack_function()
{
    while (1) {
        this_thread::sleep_for(chrono::milliseconds(200));
        if (receive_msg) {
            sleep(1);
            if (!msg_sent) {
                cout << "ack" << std::endl;
                tcp_encoder("", TH_ACK);
                msg_sent = true;
            }
        }
    }
}

int main() {
    char errbuf[PCAP_ERRBUF_SIZE];
    const char *dev = "lo"; // 可改成你要監聽的介面
    pcap_t *handle;

    std::thread main(main_function);
    main.detach();
    // std::thread ack(ack_function);
    // ack.detach();

    // 開啟裝置
    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle == nullptr) {
        cerr << "Couldn't open device " << dev << ": " << errbuf << endl;
        return 1;
    }
    cout << "Listening on " << dev << "..." << endl;
    server_address.s_addr = inet_addr("127.0.0.1");
    server_port = htons(9000);

    // 開始捕捉封包
    pcap_loop(handle, 0, packet_handler, nullptr);
    pcap_close(handle);

    return 0;
}
// g++ network_test.cpp -lpcap -o network_test
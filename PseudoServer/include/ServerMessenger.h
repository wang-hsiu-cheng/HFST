#ifndef SERVERMESSENGER_H
#define SERVERMESSENGER_H

#include <iostream>
#include <vector>
#include <memory>
#include <cstring>
#include <ctime>
#include <cerrno>
#include <iomanip>
#include <time.h> 

#include "TaifexOrder.h"
#include "TaifexOrderUtility.h"
#include "TaifexORGenerator.h"

using namespace std;

class ServerMessenger 
{
public:
    string MakeA(const std::string &buf, FIXhdr_t header, uint16_t serverSeqNum);
    string Make0(const std::string &buf, FIXhdr_t header, uint16_t serverSeqNum);
    string Make1(const std::string &buf, FIXhdr_t header, uint16_t serverSeqNum);
    string Make2(FIX_2_t *msg, FIXhdr_t header, uint16_t serverSeqNum);
    string Make2resp(const std::string &buf, FIXhdr_t header, uint16_t serverSeqNum);
    string Make3(FIX_3_t *msg, FIXhdr_t header, uint16_t serverSeqNum);
    string Make3resp(const std::string &buf, FIXhdr_t header, uint16_t serverSeqNum);
    string Make4(const std::string &buf, FIXhdr_t header, uint16_t serverSeqNum);
    string Make5(const std::string &buf, FIXhdr_t header, uint16_t serverSeqNum);
    string MakeD(const std::string &buf, FIXhdr_t header, uint16_t serverSeqNum);
    // string MakeDs(std::vector<FIX_A_t> &vec, int num);
    string MakeF(const std::string &buf, FIXhdr_t header, uint16_t serverSeqNum);
    string MakeG(const std::string &buf, FIXhdr_t header, uint16_t serverSeqNum);
    string MakeH(const std::string &buf, FIXhdr_t header, uint16_t serverSeqNum);
    string Make8(const std::string &buf, FIXhdr_t header, uint16_t serverSeqNum);
    string Make9(const std::string &buf, FIXhdr_t header, uint16_t serverSeqNum);

    string getChecksumFromFIX(const std::string& buf);
    int SearchMsgType(const std::string& buf, FIXhdr_t& header, FIX_2_t& resendRequest_msg, FIX_3_t& reject_msg);
    string DecodeMessage(const std::string &buf, std::string target_tag);
    bool ReceiveA(std::string &buf, FIX_A_t *logonMsg);
    bool ReceiveOrder(std::string &buf, std::string msgType);

    TaifexOrderUtility util;
    TaifexOrder order;
    TaifexORGenerator gen;
};

#endif
    
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
    string MakeA(uint8_t *buf, FIXhdr_t header, uint8_t status_code);
    string Make0(uint8_t *buf, FIXhdr_t header, std::string TestReqID, uint8_t status_code);
    string Make1(uint8_t *buf, FIXhdr_t header, std::string TestReqID, uint8_t status_code);
    string Make2(FIX_2_t *msg, FIXhdr_t header, uint8_t status_code);
    string Make2resp(uint8_t *buf, FIXhdr_t header, uint8_t status_code);
    string Make3(FIX_3_t *msg, FIXhdr_t header, uint8_t status_code);
    string Make3resp(uint8_t *buf, FIXhdr_t header, uint8_t status_code);
    string Make4(uint8_t *buf, FIXhdr_t header, uint16_t serverSeqNum);
    string Make5(uint8_t *buf, FIXhdr_t header, uint8_t status_code);
    string MakeD(uint8_t *buf, FIXhdr_t header, uint8_t status_code);
    string MakeDs(std::vector<FIX_A_t> &vec, int num);
    string MakeF(uint8_t *buf, FIXhdr_t header, uint8_t status_code);
    string MakeG(uint8_t *buf, FIXhdr_t header, uint8_t status_code);
    string MakeH(uint8_t *buf, FIXhdr_t header, uint8_t status_code);
    string Make8(uint8_t *buf, FIXhdr_t header, uint8_t status_code);
    string Make9(uint8_t *buf, FIXhdr_t header, uint8_t status_code);

    string getChecksumFromFIX(const std::string& buf);
    int SearchMsgType(const std::string& buf, FIXhdr_t& header, FIX_2_t& resendRequest_msg, FIX_3_t& reject_msg);
    bool ReceiveA(uint8_t *buf, FIX_A_t *logonMsg);
    bool ReceiveOrder(uint8_t *bug, std::string msgType);

    TaifexOrderUtility util;
    TaifexOrder order;
    TaifexORGenerator gen;
};

#endif
    
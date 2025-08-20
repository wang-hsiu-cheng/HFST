#ifndef SERVERMESSENGER_H
#define SERVERMESSENGER_H

#include <iostream>
#include <vector>
#include <memory>
#include <cstring>
#include <ctime>
#include <cerrno>
#include <iomanip>

#include "TaifexOrder.h"
#include "TaifexOrderUtility.h"
#include "TaifexORGenerator.h"

class ServerMessenger 
{
public:
    int MakeA(uint8_t *buf, std::string TargetCompID, uint8_t status_code);
    int Make0(uint8_t *buf, std::string TargetCompID, uint8_t status_code);
    int Make1(uint8_t *buf, std::string TargetCompID, uint8_t status_code);
    int Make2(uint8_t *buf, FIX_2_t *msg, std::string TargetCompID, uint8_t status_code);
    int Make2resp(uint8_t *buf, std::string TargetCompID, uint8_t status_code);
    int Make3(uint8_t *buf, FIX_3_t *msg, std::string TargetCompID, uint8_t status_code);
    int Make3resp(uint8_t *buf, std::string TargetCompID, uint8_t status_code);
    int Make4(uint8_t *buf, std::string TargetCompID, uint8_t status_code);
    int Make5(uint8_t *buf, std::string TargetCompID, uint8_t status_code);
    int MakeD(uint8_t *buf, std::string TargetCompID, uint8_t status_code);
    void MakeDs(std::vector<FIX_A_t> &vec, int num);
    int MakeF(uint8_t *buf, std::string TargetCompID, uint8_t status_code);
    int MakeG(uint8_t *buf, std::string TargetCompID, uint8_t status_code);
    int MakeH(uint8_t *buf, std::string TargetCompID, uint8_t status_code);
    int Make8(uint8_t *buf, std::string TargetCompID, uint8_t status_code);
    int Make9(uint8_t *buf, std::string TargetCompID, uint8_t status_code);

    std::string getChecksumFromFIX(const std::string& buf);
    int SearchMsgType(const std::string& buf, FIXhdr_t& header, FIX_2_t& resendRequest_msg, FIX_3_t& reject_msg);
    bool ReceiveA(uint8_t *buf, FIX_A_t *logonMsg);
    bool ReceiveOrder(uint8_t *bug, std::string msgType);

    TaifexOrderUtility util;
    TaifexOrder order;
};

#endif
    
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

class ServerMessenger 
{
public:
    int MakeA(uint8_t *buf, uint16_t TargetCompID, uint8_t status_code);
    int Make0(uint8_t *buf, uint16_t TargetCompID, uint8_t status_code, uint32_t endOutBoundNum);
    int Make1(uint8_t *buf, uint16_t TargetCompID, uint8_t status_code, uint32_t fileSize, const std::vector<char>& data, bool isEof);
    int Make2(uint8_t *buf, uint16_t TargetCompID, uint8_t status_code, uint8_t HeartBtInt, uint16_t max_flow_ctrl_cnt);
    int Make3(uint8_t *buf, uint16_t TargetCompID, uint8_t status_code);
    int Make4(uint8_t *buf, uint16_t TargetCompID, uint8_t status_code);
    int Make5(uint8_t *buf, uint16_t TargetCompID, uint8_t status_code);
    int MakeD(uint8_t *buf, uint16_t TargetCompID, uint8_t status_code);
    int MakeG(uint8_t *buf, uint16_t TargetCompID, uint8_t status_code, uint16_t append_no);
    int MakeH(uint8_t *buf, uint16_t TargetCompID, uint8_t status_code);
    int Make8(uint8_t *buf, uint16_t TargetCompID, uint8_t status_code);
    int Make9(uint8_t *buf, uint16_t TargetCompID, uint8_t status_code);

    char SearchMsgType(uint8_t* buf);

    TaifexOrderUtility util;
};

#endif
    
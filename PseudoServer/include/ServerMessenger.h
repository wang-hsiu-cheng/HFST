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
    int MakeL10(uint8_t *buf, uint16_t fcm_id, uint16_t session_id, uint8_t status_code);
    int MakeL30(uint8_t *buf, uint16_t fcm_id, uint16_t session_id, uint8_t status_code, uint32_t endOutBoundNum);
    int MakeL41(uint8_t *buf, uint16_t fcm_id, uint16_t session_id, uint8_t status_code, uint32_t fileSize, const std::vector<char>& data, bool isEof);
    int MakeL50(uint8_t *buf, uint16_t fcm_id, uint16_t session_id, uint8_t status_code, uint8_t HeartBtInt, uint16_t max_flow_ctrl_cnt);
    int MakeR12(uint8_t *buf, uint16_t fcm_id, uint16_t session_id, uint8_t status_code);
    int MakeR04(uint8_t *buf, uint16_t fcm_id, uint16_t session_id, uint8_t status_code);
    int MakeR05(uint8_t *buf, uint16_t fcm_id, uint16_t session_id, uint8_t status_code);

    int MakeL20(uint8_t *buf, uint16_t fcm_id, uint16_t session_id, uint8_t status_code);
    int MakeL40(uint8_t *buf, uint16_t fcm_id, uint16_t session_id, uint8_t status_code, uint16_t append_no);
    int MakeL60(uint8_t *buf, uint16_t fcm_id, uint16_t session_id, uint8_t status_code);

    int IsRecvL10(uint8_t* buf);
    int IsRecvL20(uint8_t* buf);
    int IsRecvL40(uint8_t* buf);
    int IsRecvL42(uint8_t* buf);
    int IsRecvL60(uint8_t* buf);
    int IsRecvR11(uint8_t* buf);
    int IsRecvR05(uint8_t* buf);

    TaifexOrderUtility util;
};

#endif
    
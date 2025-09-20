#ifndef TAIFEX_ORDER_UTILITY_H
#define TAIFEX_ORDER_UTILITY_H

#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
#include <cstring>
#include <ctime>
#include <cerrno>
#include <chrono>
#include <iomanip>
#include <sys/socket.h>

#include "TaifexOrder.h"

class TaifexOrderUtility 
{
public:
    // void FIXHdrSet(FIXhdr_t* hdr, uint16_t msgType, uint16_t msgLen);

    std::string FIXComputeCheckSum(const std::string& data);

    int GetMsgLen(FIXhdr_t* hdr);

    std::string GetMsg_time_tStr(Msg_time_t t);

    Msg_time_t GetMsg_time_t(const std::string& timeStr);
    Msg_time_t GetMsg_time_t();

    char* GetTimeStr(time_t msg_time_sec, uint16_t msg_time_ms);
    
    void PrintFIXPkt(FILE* fptr, const void* data);
    void HexDump(FILE *fptr, const char *desc, void *addr, int len);
    
private:
    char timestr[50];
    char dateString [64];
    const char *Weekdays[7] = {
        "Sunday",
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday",
        "Saturday"
    };
    TaifexOrder order;
};

#endif

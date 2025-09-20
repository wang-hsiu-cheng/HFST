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
    std::string FIXComputeCheckSum(const std::string& data);

    std::string GetMsg_time_tStr(Msg_time_t t);
    Msg_time_t GetMsg_time_t(const std::string& timeStr);
    Msg_time_t GetMsg_time_t();
    
    void PrintFIXPkt(FILE* fptr, const void* data);
    
private:
    char dateString [64];
    TaifexOrder order;
};

#endif

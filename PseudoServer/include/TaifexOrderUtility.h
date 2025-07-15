#ifndef TAIFEX_ORDER_UTILITY_H
#define TAIFEX_ORDER_UTILITY_H

#include <iostream>
#include <vector>
#include <memory>
#include <cstring>
#include <ctime>
#include <cerrno>
#include <iomanip>
#include <sys/socket.h>

#include "TaifexOrder.h"

class TaifexOrderUtility 
{
public:
    void TMPHdrSet(TMPhdr_t* hdr, uint16_t msgType, uint16_t msgLen);

    void TMPSetCheckSum(uint8_t* checksum, const void* data, size_t size);

    int GetMsgLen(TMPhdr_t* hdr);

    char* TMPTimeToString(time_t curSec);

    char* GetTimeStr(time_t msg_time_sec, uint16_t msg_time_ms);
    
    void PrintTMPPkt(FILE* fptr, const void* data);
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

};

#endif
#ifndef TAIFEX_ORDER_GENERATOR_H
#define TAIFEX_ORDER_GENERATOR_H

#include <iostream>
#include <vector>
#include <memory>
#include <cstring>
#include <map>
#include <cmath>
#include <algorithm>

#include "TaifexOrder.h"
#include "TaifexOrderUtility.h"

class TaifexORGenerator
{
public:
    std::string SerializeHeader(const FIXhdr_t& msg);
    std::string SerializeA(const FIX_A_t& msg);
    std::string Serialize0(const FIX_0_t& msg);
    std::string Serialize1(const FIX_1_t& msg);
    std::string Serialize2(const FIX_2_t& msg);
    std::string Serialize3(const FIX_3_t& msg);
    std::string Serialize4(const FIX_4_t& msg);
    std::string Serialize5(const FIX_5_t& msg);
    std::string SerializeD(const FIX_D_t& msg);
    std::string SerializeF(const FIX_F_t& msg);
    std::string SerializeG(const FIX_G_t& msg);
    std::string SerializeH(const FIX_H_t& msg);
    std::string Serialize8(const FIX_8_t& msg);
    std::string Serialize9(const FIX_9_t& msg);
    std::string Serializej(const FIX_j_t& msg);

    inline int GetPassword(std::string SenderCompID)
    {
        return password[SenderCompID];
    }

    TaifexOrderUtility util;

private:
    uint64_t OrdernoToNum(std::string orderno);
    std::string NumToOrderno(uint64_t num);

    std::map<std::string, int> password = {
        {"T116001", 0001},
        {"T116002", 0002},
        {"T116003", 0003},
        {"T116004", 0004},
        {"T116005", 0005}
    };

    TaifexOrder order;
};


#endif


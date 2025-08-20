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
    void AddSessionID(uint16_t v);

    void SetBeginMsgSeqNum(uint32_t v);
    void SetOrderType(uint8_t v);
    void SetFCMID(uint16_t v);
    void SetCMID(uint16_t v);

    void SetUserDefine(char *v);
    void SetInvestorAcno(uint32_t v);
    void SetInvestorFlag(char v);
    void SetPositionEffect(char v);
    void SetOrderSource(char v);
    void SetInfoSource(char *v);
    
    void SetOrderPriceD100Range(uint32_t begin, uint32_t end);
    void SetOrderQtyRange(uint32_t begin, uint32_t end);
    void SetOrdernoRange(std::string begin, std::string end);
    void SetOrderSymbols(std::vector<std::string> vec_symbols);

    // void GenSingleR01(FIX_R01_t *p, char in_execType);
    // void GenR01s(std::vector<FIX_R01_t> &vec, int num);

    // void GenSingleR09(FIX_R09_t *p, char in_execType);
    // void GenR09s(std::vector<FIX_R09_t> &vec, int num);

    // void GenReport(FIX_R01_t *src_order, std::vector<FIX_R02_t> &vec_r02, std::vector<FIX_R03_t> &vec_r03);
    // void GenReport(FIX_R09_t *src_order, std::vector<FIX_R02_t> &vec_r02, std::vector<FIX_R03_t> &vec_r03);

    
    // std::vector<FIX_R01_t>* GetR01New() { return &m_vec_new_r01; }
    // std::vector<FIX_R01_t>* GetR01Cxl() { return &m_vec_cxl_r01; }
    
    // std::vector<FIX_R09_t>* GetR09New() { return &m_vec_new_r09; }
    // std::vector<FIX_R09_t>* GetR09Cxl() { return &m_vec_cxl_r09; }

    TaifexOrderUtility util;

private:
    // void GenR02(FIX_R01_t *src_order, std::vector<FIX_R02_t> &vec_r02);
    // void GenR03(FIX_R01_t *src_order, std::vector<FIX_R03_t> &vec_r03);

    // void GenR02(FIX_R09_t *src_order, int side, std::vector<FIX_R02_t> &vec_r02);
    // void GenR03(FIX_R09_t *src_order, int side, std::vector<FIX_R03_t> &vec_r03);

    uint64_t OrdernoToNum(std::string orderno);
    std::string NumToOrderno(uint64_t num);

    uint32_t m_order_beginMsgSeqNum;
    uint32_t m_report_beginMsgSeqNum = 1;
    uint8_t m_orderType;

    uint16_t m_fcmID;
    uint16_t m_cmID;

    char m_userDefine[8];
    uint32_t m_investorAcno;
    char m_investorFlag;
    char m_positionEffect;
    char m_orderSource;
    char m_infoSource[3];

    uint32_t m_orderPriceBegin_d100;
    uint32_t m_orderPriceEnd_d100;
    uint32_t m_orderQtyBegin;
    uint32_t m_orderQtyEnd;
    std::string m_ordernoBegin;
    std::string m_ordernoEnd;
    uint64_t m_ordernoBeginNum;
    uint64_t m_ordernoEndNum;
    std::vector<std::string> m_vec_symbols;
    
    std::vector<uint16_t> m_vec_sessionID;

    uint32_t m_order_id = 1;

    std::vector<FIX_D_t> m_vec_new_D, m_vec_cxl_D;

    std::map<std::string, FIX_D_t> m_map_orderno_new_D, m_map_orderno_cxl_D;

    std::map<std::string, FIX_8_t> m_map_orderno_8;
    std::map<std::string, FIX_9_t> m_map_orderno_9;


    uint8_t m_order_exectype[2] = {'0', '4'};
    uint8_t m_timeInForce[3] = {0, 3, 4};

    const uint8_t m_alph[62] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
        'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
        'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
        'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
        'y', 'z'
    };

    std::map<uint8_t, uint8_t> m_map_alph_num = {
    {'0', 0}, {'1', 1}, {'2', 2}, {'3', 3}, {'4', 4}, {'5', 5}, {'6', 6}, {'7', 7}, {'8', 8}, {'9', 9},
    {'A', 10}, {'B', 11}, {'C', 12}, {'D', 13}, {'E', 14}, {'F', 15}, {'G', 16}, {'H', 17}, {'I', 18}, {'J', 19},
    {'K', 20}, {'L', 21}, {'M', 22}, {'N', 23}, {'O', 24}, {'P', 25}, {'Q', 26}, {'R', 27}, {'S', 28}, {'T', 29},
    {'U', 30}, {'V', 31}, {'W', 32}, {'X', 33}, {'Y', 34}, {'Z', 35}, {'a', 36}, {'b', 37}, {'c', 38}, {'d', 39},
    {'e', 40}, {'f', 41}, {'g', 42}, {'h', 43}, {'i', 44}, {'j', 45}, {'k', 46}, {'l', 47}, {'m', 48}, {'n', 49},
    {'o', 50}, {'p', 51}, {'q', 52}, {'r', 53}, {'s', 54}, {'t', 55}, {'u', 56}, {'v', 57}, {'w', 58}, {'x', 59},
    {'y', 60}, {'z', 61}
    };

    std::map<uint8_t, uint8_t> m_map_num_alph = {
    {0, '0'}, {1, '1'}, {2, '2'}, {3, '3'}, {4, '4'}, {5, '5'}, {6, '6'}, {7, '7'}, {8, '8'}, {9, '9'},
    {10, 'A'}, {11, 'B'}, {12, 'C'}, {13, 'D'}, {14, 'E'}, {15, 'F'}, {16, 'G'}, {17, 'H'}, {18, 'I'}, {19, 'J'},
    {20, 'K'}, {21, 'L'}, {22, 'M'}, {23, 'N'}, {24, 'O'}, {25, 'P'}, {26, 'Q'}, {27, 'R'}, {28, 'S'}, {29, 'T'},
    {30, 'U'}, {31, 'V'}, {32, 'W'}, {33, 'X'}, {34, 'Y'}, {35, 'Z'}, {36, 'a'}, {37, 'b'}, {38, 'c'}, {39, 'd'},
    {40, 'e'}, {41, 'f'}, {42, 'g'}, {43, 'h'}, {44, 'i'}, {45, 'j'}, {46, 'k'}, {47, 'l'}, {48, 'm'}, {49, 'n'},
    {50, 'o'}, {51, 'p'}, {52, 'q'}, {53, 'r'}, {54, 's'}, {55, 't'}, {56, 'u'}, {57, 'v'}, {58, 'w'}, {59, 'x'},
    {60, 'y'}, {61, 'z'}
    };

    TaifexOrder order;
};


#endif


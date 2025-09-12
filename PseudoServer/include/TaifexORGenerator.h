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
    // void AddSessionID(uint16_t v);

    // void SetBeginMsgSeqNum(uint32_t v);
    // void SetOrderType(uint8_t v);
    // void SetFCMID(uint16_t v);
    // void SetCMID(uint16_t v);

    // void SetUserDefine(char *v);
    // void SetInvestorAcno(uint32_t v);
    // void SetInvestorFlag(char v);
    // void SetPositionEffect(char v);
    // void SetOrderSource(char v);
    // void SetInfoSource(char *v);
    
    // void SetOrderPriceD100Range(uint32_t begin, uint32_t end);
    // void SetOrderQtyRange(uint32_t begin, uint32_t end);
    // void SetOrdernoRange(std::string begin, std::string end);
    // void SetOrderSymbols(std::vector<std::string> vec_symbols);

    inline int GetPassword(std::string SenderCompID)
    {
        return password[SenderCompID];
    }

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


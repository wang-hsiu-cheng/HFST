#include "TaifexORGenerator.h"

// void TaifexORGenerator::AddSessionID(uint16_t v)
// {
//     if(std::find(m_vec_sessionID.begin(), m_vec_sessionID.end(), v) == m_vec_sessionID.end())
//     {
//         m_vec_sessionID.push_back(v);
//     }
// }

// void TaifexORGenerator::SetBeginMsgSeqNum(uint32_t v)
// {
//     m_order_beginMsgSeqNum = v;
// }

// void TaifexORGenerator::SetOrderType(uint8_t v)
// {
//     m_orderType = v;
// }

// void TaifexORGenerator::SetFCMID(uint16_t v)
// {
//     m_fcmID = v;
// }

// void TaifexORGenerator::SetCMID(uint16_t v)
// {
//     m_cmID = v;
// }

// void TaifexORGenerator::SetUserDefine(char *v)
// {
//     memcpy(m_userDefine, v, FIX_UDD_LEN);
// }

// void TaifexORGenerator::SetInvestorAcno(uint32_t v)
// {
//     m_investorAcno = v;
// }

// void TaifexORGenerator::SetInvestorFlag(char v)
// {
//     m_investorFlag = v;
// }

// void TaifexORGenerator::SetPositionEffect(char v)
// {
//     m_positionEffect = v;
// }

// void TaifexORGenerator::SetOrderSource(char v)
// {
//     m_orderSource = v;
// }

// void TaifexORGenerator::SetInfoSource(char *v)
// {
//     memcpy(m_infoSource, v, 3);
// }

// void TaifexORGenerator::SetOrderPriceD100Range(uint32_t begin, uint32_t end)
// {
//     m_orderPriceBegin_d100 = begin;
//     m_orderPriceEnd_d100 = end;
// }

// void TaifexORGenerator::SetOrderQtyRange(uint32_t begin, uint32_t end)
// {
//     m_orderQtyBegin = begin;
//     m_orderQtyEnd = end;
// }

// void TaifexORGenerator::SetOrdernoRange(std::string begin, std::string end)
// {
//     m_ordernoBegin = begin;
//     m_ordernoEnd = end;

//     m_ordernoBeginNum = OrdernoToNum(begin);
//     m_ordernoEndNum = OrdernoToNum(end);
// }

// void TaifexORGenerator::SetOrderSymbols(std::vector<std::string> vec_symbols)
// {
//     m_vec_symbols = vec_symbols;
// }

// void TaifexORGenerator::GenReport(FIX_R01_t *src_order, std::vector<FIX_R02_t> &vec_r02, std::vector<FIX_R03_t> &vec_r03)
// {
//     int pb = rand() % 100;
//     if(src_order->ExecType == '0')
//     {
//         if(pb < 75)
//         {
//             GenR02(src_order, vec_r02);
//         }
//         else
//         {
//             GenR03(src_order, vec_r03);
//         }
//     }
//     else if(src_order->ExecType == '4')
//     {
//         if(pb < 75)
//         {
//             GenR02(src_order, vec_r02);
//         }
//         else
//         {
//             GenR03(src_order, vec_r03);
//         }
//     }
// }

// void TaifexORGenerator::GenReport(FIX_R09_t *src_order, std::vector<FIX_R02_t> &vec_r02, std::vector<FIX_R03_t> &vec_r03)
// {
//     for(int i = 0; i < 2; i++)
//     {
//         int pb = rand() % 100;
//         if(src_order->ExecType == '0')
//         {
//             if(pb < 75)
//             {
//                 GenR02(src_order, i + 1, vec_r02);
//             }
//             else
//             {
//                 GenR03(src_order, i + 1, vec_r03);
//             }
//         }
//         else if(src_order->ExecType == '4')
//         {
//             if(pb < 75)
//             {
//                 GenR02(src_order, i + 1, vec_r02);
//             }
//             else
//             {
//                 GenR03(src_order, i + 1, vec_r03);
//             }
//         }
//     }
// }

// uint64_t TaifexORGenerator::OrdernoToNum(std::string orderno)
// {
//     if(orderno.size() != FIX_ORDNO_LEN)
//         return 0;

//     uint64_t num = 0;
//     for(int i = 0; i < FIX_ORDNO_LEN; i++)
//     {
//         uint8_t c = orderno[i];
//         num += m_map_alph_num[c] * (uint64_t)pow(62, FIX_ORDNO_LEN - i - 1);
//     }

//     return num;
// }

// std::string TaifexORGenerator::NumToOrderno(uint64_t num)
// {
//     std::string orderno(FIX_ORDNO_LEN, '0');
//     for(int i = 0; i < FIX_ORDNO_LEN - 1; i++)
//     {
//         uint64_t base = (uint64_t)pow(62, FIX_ORDNO_LEN - i - 1);
//         uint64_t quotient = num / base;
//         orderno[i] = m_map_num_alph[(uint8_t)quotient];
//         num -= quotient * base;
//     }

//     orderno[FIX_ORDNO_LEN - 1] = m_map_num_alph[(uint8_t)num];

//     return orderno;
// }

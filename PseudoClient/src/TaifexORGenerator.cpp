#include "TaifexORGenerator.h"

std::string TaifexORGenerator::SerializeHeader(const FIXhdr_t& msg) {
    std::ostringstream oss;
    oss << "35=" << msg.MessageType   << '\x01';
    oss << "34=" << msg.MsgSeqNum     << '\x01';
    oss << "49=" << msg.SenderCompID  << '\x01';
    oss << "56=" << msg.TargetCompID  << '\x01';
    oss << "52=" << util.GetMsg_time_tStr(msg.SendingTime) << '\x01';
    return oss.str();
}

std::string TaifexORGenerator::SerializeA(const FIX_A_t& msg) {
    std::ostringstream oss;
    oss << SerializeHeader(msg.hdr);
    oss << "98="  << msg.EncryptMethod   << '\x01';
    oss << "108="  << msg.HeartBtInt    << '\x01';
    oss << "95=" << msg.RawDataLength       << '\x01';
    oss << "96=" << msg.RawData     << '\x01';
    return oss.str();
}

std::string TaifexORGenerator::Serialize0(const FIX_0_t& msg) {
    std::ostringstream oss;
    oss << SerializeHeader(msg.hdr);
    oss << "112="  << msg.TestReqID   << '\x01';
    return oss.str();
}

std::string TaifexORGenerator::Serialize1(const FIX_1_t& msg) {
    std::ostringstream oss;
    oss << SerializeHeader(msg.hdr);
    oss << "112="  << msg.TestReqID   << '\x01';
    return oss.str();
}

std::string TaifexORGenerator::Serialize2(const FIX_2_t& msg) {
    std::ostringstream oss;
    oss << SerializeHeader(msg.hdr);
    oss << "7="  << msg.BeginSeqNo   << '\x01';
    oss << "16="  << msg.EndSeqNo    << '\x01';
    return oss.str();
}

std::string TaifexORGenerator::Serialize3(const FIX_3_t& msg) {
    std::ostringstream oss;
    oss << SerializeHeader(msg.hdr);
    oss << "45="  << msg.RefSeqNum   << '\x01';
    oss << "371="  << msg.RefTagID    << '\x01';
    oss << "372=" << msg.RefMsgType       << '\x01';
    oss << "373=" << msg.SessionRejectReason     << '\x01';
    oss << "58=" << msg.Text     << '\x01';
    return oss.str();
}

std::string TaifexORGenerator::Serialize4(const FIX_4_t& msg) {
    std::ostringstream oss;
    oss << SerializeHeader(msg.hdr);
    oss << "123="  << msg.GapFillFlag   << '\x01';
    oss << "36="  << msg.NewSeqNo    << '\x01';
    return oss.str();
}

std::string TaifexORGenerator::Serialize5(const FIX_5_t& msg) {
    std::ostringstream oss;
    oss << SerializeHeader(msg.hdr);
    oss << "58="  << msg.Text   << '\x01';
    return oss.str();
}

std::string TaifexORGenerator::SerializeD(const FIX_D_t& msg) {
    std::ostringstream oss;
    oss << SerializeHeader(msg.hdr);
    oss << "11="  << msg.ClOrdID   << '\x01';
    oss << "37="  << msg.OrderID    << '\x01';
    oss << "1=" << msg.Account       << '\x01';
    oss << "55=" << msg.Symbol     << '\x01';
    oss << "54="  << msg.Side   << '\x01';
    oss << "60="  << util.GetMsg_time_tStr(msg.TransactTime)    << '\x01';
    oss << "38=" << msg.OrderQty       << '\x01';
    oss << "40=" << msg.OrderType     << '\x01';
    oss << "59="  << msg.TimeInFource   << '\x01';
    oss << "44="  << msg.Price    << '\x01';
    oss << "10000=" << msg.TwselvacnoFlag       << '\x01';
    oss << "10001=" << msg.TwseOrdType     << '\x01';
    oss << "10002=" << msg.TwseExCode       << '\x01';
    oss << "10004=" << msg.TwseRejStaleOrd     << '\x01';
    return oss.str();
}

std::string TaifexORGenerator::SerializeF(const FIX_F_t& msg) {
    std::ostringstream oss;
    oss << SerializeHeader(msg.hdr);
    oss << "11="  << msg.ClOrdID   << '\x01';
    oss << "41="  << msg.OrigClOrdID    << '\x01';
    oss << "37=" << msg.OrderID       << '\x01';
    oss << "1=" << msg.Account     << '\x01';
    oss << "55="  << msg.Symbol   << '\x01';
    oss << "54="  << msg.Side    << '\x01';
    oss << "60=" << util.GetMsg_time_tStr(msg.TransactTime)       << '\x01';
    oss << "10000=" << msg.TwselvacnoFlag     << '\x01';
    oss << "10002=" << msg.TwseExCode       << '\x01';
    oss << "10004=" << msg.TwseRejStaleOrd     << '\x01';
    return oss.str();
}

std::string TaifexORGenerator::SerializeG(const FIX_G_t& msg) {
    std::ostringstream oss;
    oss << SerializeHeader(msg.hdr);
    oss << "11="  << msg.ClOrdID   << '\x01';
    oss << "41="  << msg.OrigClOrdID    << '\x01';
    oss << "37=" << msg.OrderID       << '\x01';
    oss << "1=" << msg.Account     << '\x01';
    oss << "55="  << msg.Symbol   << '\x01';
    oss << "54="  << msg.Side    << '\x01';
    oss << "60=" << util.GetMsg_time_tStr(msg.TransactTime)       << '\x01';
    oss << "38=" << msg.OrderQty     << '\x01';
    oss << "44="  << msg.Price   << '\x01';
    oss << "10000="  << msg.TwselvacnoFlag    << '\x01';
    oss << "10001=" << msg.TwseOrdType       << '\x01';
    oss << "10002=" << msg.TwseExCode     << '\x01';
    return oss.str();
}

std::string TaifexORGenerator::SerializeH(const FIX_H_t& msg) {
    std::ostringstream oss;
    oss << SerializeHeader(msg.hdr);
    oss << "11="  << msg.ClOrdID   << '\x01';
    oss << "37="  << msg.OrderID    << '\x01';
    oss << "55=" << msg.Symbol       << '\x01';
    oss << "54=" << msg.Side     << '\x01';
    oss << "10000=" << msg.TwselvacnoFlag       << '\x01';
    oss << "10002=" << msg.TwseExCode     << '\x01';
    return oss.str();
}

std::string TaifexORGenerator::Serialize8(const FIX_8_t& msg) {
    std::ostringstream oss;
    oss << SerializeHeader(msg.hdr);
    oss << "37="  << msg.OrderID    << '\x01';
    oss << "11="  << msg.ClOrdID    << '\x01';
    oss << "41=" << msg.OrigClOrdID << '\x01';
    oss << "17=" << msg.ExecID      << '\x01';
    oss << "150="  << msg.ExecType  << '\x01';
    oss << "39="  << msg.OrdStatus  << '\x01';
    oss << "103=" << msg.OrdRejReason       << '\x01';
    oss << "1=" << msg.Account      << '\x01';
    oss << "55="  << msg.Symbol     << '\x01';
    oss << "54="  << msg.Side       << '\x01';
    oss << "60=" << util.GetMsg_time_tStr(msg.TransactTime)       << '\x01';
    oss << "38=" << msg.OrderQty    << '\x01';
    oss << "40="  << msg.OrdType    << '\x01';
    oss << "59="  << msg.TimeInForce    << '\x01';
    oss << "44=" << msg.Price       << '\x01';
    oss << "32=" << msg.LastQty     << '\x01';
    oss << "31=" << msg.LastPx      << '\x01';
    oss << "151="  << msg.LeavesQty << '\x01';
    oss << "14="  << msg.CumQty     << '\x01';
    oss << "6=" << msg.AvgPx        << '\x01';
    oss << "58=" << msg.Text        << '\x01';
    oss << "10000="  << msg.TwseIvacnoFlag    << '\x01';
    oss << "10001=" << msg.TwseOrdType       << '\x01';
    oss << "10002=" << msg.TwseExCode     << '\x01';
    return oss.str();
}

std::string TaifexORGenerator::Serialize9(const FIX_9_t& msg) {
    std::ostringstream oss;
    oss << SerializeHeader(msg.hdr);
    oss << "37="  << msg.OrderID   << '\x01';
    oss << "11="  << msg.ClOrdID    << '\x01';
    oss << "41=" << msg.OrigClOrdID       << '\x01';
    oss << "39=" << msg.OrdStatus     << '\x01';
    oss << "1="  << msg.Account   << '\x01';
    oss << "60="  << util.GetMsg_time_tStr(msg.TransactTime)    << '\x01';
    oss << "434=" << msg.CxlRejResponseTo       << '\x01';
    oss << "102=" << msg.CxlRejReason     << '\x01';
    oss << "58="  << msg.Text   << '\x01';
    return oss.str();
}

std::string TaifexORGenerator::Serializej(const FIX_j_t& msg) {
    std::ostringstream oss;
    oss << SerializeHeader(msg.hdr);
    oss << "45="  << msg.RefSeqNum   << '\x01';
    oss << "372=" << msg.RefMsgType    << '\x01';
    oss << "380=" << msg.BusinessRejectReason       << '\x01';
    oss << "58="  << msg.Text     << '\x01';
    return oss.str();
}

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

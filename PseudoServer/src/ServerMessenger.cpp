#include "ServerMessenger.h"

int ServerMessenger::MakeA(uint8_t *buf, std::string TargetCompID, uint8_t status_code)
{
    std::string buf_str(reinterpret_cast<char*>(buf));
    // util.FIXHdrSet(&p->MsgHeader, FIXMsgType_A, sizeof(FIX_A_t));
    // FIXSET_UINT16(p->MsgHeader.TargetCompID, TargetCompID);
    // FIXSET_UINT16(p->MsgHeader.session_id, session_id);
    // p->status_code = status_code;
    // p->start_in_bound_num = 0;
    util.FIXComputeCheckSum(buf_str);

    return sizeof(FIX_A_t);
}

int ServerMessenger::Make0(uint8_t *buf, std::string TargetCompID, uint8_t status_code) 
{
    std::string buf_str(reinterpret_cast<char*>(buf));
    // util.FIXHdrSet(&p->MsgHeader, FIXMsgType_0, sizeof(FIX_0_t));
    // FIXSET_UINT16(p->MsgHeader.TargetCompID, TargetCompID);
    // FIXSET_UINT16(p->MsgHeader.session_id, session_id);
    // p->status_code = status_code;
    // p->end_in_bound_num = endOutBoundNum;
    // p->EncryptMethod = 0;
    util.FIXComputeCheckSum(buf_str);

    return sizeof(FIX_0_t);
}

int ServerMessenger::Make1(uint8_t *buf, std::string TargetCompID, uint8_t status_code) 
{
    std::string buf_str(reinterpret_cast<char*>(buf));
    // util.FIXHdrSet(&p->MsgHeader, FIXMsgType_1, sizeof(FIX_1_t) + data.size());
    // FIXSET_UINT16(p->MsgHeader.TargetCompID, TargetCompID);
    // FIXSET_UINT16(p->MsgHeader.session_id, session_id);
    // p->status_code = status_code;
    // p->is_eof = isEof ? 1 : 0;
    // p->file_size = fileSize;
    util.FIXComputeCheckSum(buf_str);
    // memcpy(p->data, data.data(), data.size());

    return sizeof(FIX_1_t);
}

int ServerMessenger::Make2(uint8_t *buf, FIX_2_t *msg, std::string TargetCompID, uint8_t status_code) 
{
    std::string buf_str(reinterpret_cast<char*>(buf));
    // util.FIXHdrSet(&p->MsgHeader, FIXMsgType_2, sizeof(FIX_2_t));
    // FIXSET_UINT16(p->MsgHeader.TargetCompID, TargetCompID);
    // FIXSET_UINT16(p->MsgHeader.session_id, session_id);
    // p->status_code = status_code;
    // p->HeartBtInt = HeartBtInt;
    // p->max_flow_ctrl_cnt = max_flow_ctrl_cnt;
    util.FIXComputeCheckSum(buf_str);
    
    return sizeof(FIX_2_t);
    // FIX_R03_t r03;

    // memset(&r03, 0, sizeof(FIX_R03_t));

    // util.FIXHdrSet(&r03.MsgHeader, FIXMsgType_R03, sizeof(FIX_R03_t));
    // FIXSET_UINT32(r03.MsgHeader.MsgSeqNum, m_report_beginMsgSeqNum++);
    // r03.MsgHeader.TargetCompID = src_order->MsgHeader.TargetCompID;
    // r03.MsgHeader.session_id = src_order->MsgHeader.session_id;

    // if(src_order->ExecType == '4')//cancel
    // {
    //     r03.ExecType = '4';
    // }
    // else
    // {
    //     r03.ExecType = '0';
    // }

    // r03.status_code = rand() % 46 + 1;
    // r03.TargetCompID = src_order->TargetCompID;
    // FIXSET_CHAR_N(r03.order_no, src_order->order_no, FIX_ORDNO_LEN);
    // r03.ord_id = src_order->ord_id;
    // FIXSET_CHAR_N(r03.user_define, src_order->user_define, FIX_UDD_LEN);

    // r03.Side = side;

    // util.FIXSetCheckSum(&r03.CheckSum, (uint8_t*)&r03, sizeof(FIX_R03_t));
    // vec_report.push_back(r03);
}

int ServerMessenger::Make2resp(uint8_t *buf, std::string TargetCompID, uint8_t status_code) 
{
    std::string buf_str(reinterpret_cast<char*>(buf));
    return sizeof(FIX_D_t);
}

int ServerMessenger::Make3(uint8_t *buf, FIX_3_t *msg, std::string TargetCompID, uint8_t status_code) 
{
    std::string buf_str(reinterpret_cast<char*>(buf));
    // util.FIXHdrSet(&p->MsgHeader, FIXMsgType_3, sizeof(FIX_3_t));
    // FIXSET_UINT16(p->MsgHeader.TargetCompID, TargetCompID);
    // FIXSET_UINT16(p->MsgHeader.session_id, session_id);
    // p->status_code = status_code;
    util.FIXComputeCheckSum(buf_str);

    return sizeof(FIX_3_t);
    // FIX_R03_t r03;

    // memset(&r03, 0, sizeof(FIX_R03_t));

    // util.FIXHdrSet(&r03.MsgHeader, FIXMsgType_R03, sizeof(FIX_R03_t));
    // FIXSET_UINT32(r03.MsgHeader.MsgSeqNum, m_report_beginMsgSeqNum++);
    // r03.MsgHeader.TargetCompID = src_order->MsgHeader.TargetCompID;
    // r03.MsgHeader.session_id = src_order->MsgHeader.session_id;

    // if(src_order->ExecType == '4')//cancel
    // {
    //     r03.ExecType = '4';
    // }
    // else
    // {
    //     r03.ExecType = '0';
    // }

    // r03.status_code = rand() % 46 + 1;
    // r03.TargetCompID = src_order->TargetCompID;
    // FIXSET_CHAR_N(r03.order_no, src_order->order_no, FIX_ORDNO_LEN);
    // r03.ord_id = src_order->ord_id;
    // FIXSET_CHAR_N(r03.user_define, src_order->user_define, FIX_UDD_LEN);

    // r03.Side = side;

    // util.FIXSetCheckSum(&r03.CheckSum, (uint8_t*)&r03, sizeof(FIX_R03_t));
    // vec_report.push_back(r03);
}

int ServerMessenger::Make3resp(uint8_t *buf, std::string TargetCompID, uint8_t status_code) 
{
    std::string buf_str(reinterpret_cast<char*>(buf));
    return sizeof(FIX_D_t);
}

int ServerMessenger::Make4(uint8_t *buf, std::string TargetCompID, uint8_t status_code) 
{
    std::string buf_str(reinterpret_cast<char*>(buf));
    // util.FIXHdrSet(&p->MsgHeader, FIXMsgType_4, sizeof(FIX_4_t));
    // FIXSET_UINT16(p->MsgHeader.TargetCompID, TargetCompID);
    // FIXSET_UINT16(p->MsgHeader.session_id, session_id);
    // p->status_code = status_code;
    util.FIXComputeCheckSum(buf_str);

    return sizeof(FIX_4_t);
}

int ServerMessenger::Make5(uint8_t *buf, std::string TargetCompID, uint8_t status_code) 
{
    std::string buf_str(reinterpret_cast<char*>(buf));
    // util.FIXHdrSet(&p->MsgHeader, FIXMsgType_5, sizeof(FIX_5_t));
    // FIXSET_UINT16(p->MsgHeader.TargetCompID, TargetCompID);
    // FIXSET_UINT16(p->MsgHeader.session_id, session_id);
    // p->status_code = status_code;
    util.FIXComputeCheckSum(buf_str);

    return sizeof(FIX_5_t);
}

int ServerMessenger::MakeD(uint8_t *buf, std::string TargetCompID, uint8_t status_code) 
{
    std::string buf_str(reinterpret_cast<char*>(buf));
    // util.FIXHdrSet(&p->MsgHeader, FIXMsgType_D, sizeof(FIX_D_t));
    // FIXSET_UINT16(p->MsgHeader.TargetCompID, TargetCompID);
    // FIXSET_UINT16(p->MsgHeader.session_id, session_id);
    // p->status_code = status_code;
    util.FIXComputeCheckSum(buf_str);

    return sizeof(FIX_D_t);

    // R01
    // int price = 0;
    // uint16_t qty = 0;
    // uint8_t execType = in_execType, side = 0;
    // std::string sym = "";
    // std::string orderno = "";

    // //header
    // util.FIXHdrSet(&p->MsgHeader, FIXMsgType_R01, sizeof(FIX_R01_t));
    // FIXSET_UINT32(p->MsgHeader.MsgSeqNum, m_order_beginMsgSeqNum++);
    // FIXSET_UINT16(p->MsgHeader.TargetCompID, m_fcmID);
    // FIXSET_UINT16(p->MsgHeader.session_id, m_vec_sessionID[rand() % m_vec_sessionID.size()]);

    // //body
    // if(execType == '4')
    // {
    //     if(m_vec_new_r01.size() > 0)
    //     {
    //         FIX_R01_t new_order = m_vec_new_r01[rand() % m_vec_new_r01.size()];
    //         side = new_order.Side;
    //         sym = std::string(new_order.sym, FIX_SYM_LEN);
    //         orderno = std::string(new_order.order_no, FIX_ORDNO_LEN);
    //     }
    // }
    
    // if(orderno == "")
    // {
    //     execType = '0';

    //     orderno = NumToOrderno(m_ordernoBeginNum++).c_str();
    //     sym = m_vec_symbols[rand() % m_vec_symbols.size()];

    //     if(sym.size() < FIX_SYM_LEN)
    //     {
    //         sym.append(FIX_SYM_LEN - sym.size(), ' ');
    //     }

    //     price = (m_orderPriceBegin_d100 + rand() % (m_orderPriceEnd_d100 - m_orderPriceBegin_d100)) * 100;
    //     qty = m_orderQtyBegin + rand() % (m_orderQtyEnd - m_orderQtyBegin);
    //     side = rand() % 2 + 1;
    // }
    
    // FIXSET_CHAR_N(&p->ExecType, &execType, 1);
    // FIXSET_UINT16(p->cm_id, m_cmID);
    // FIXSET_UINT16(p->TargetCompID, m_fcmID);
    // FIXSET_CHAR_N(p->order_no, orderno.c_str(), FIX_ORDNO_LEN);
    // FIXSET_UINT32(p->ord_id, m_order_id++);
    // FIXSET_CHAR_N(p->user_define, m_userDefine, FIX_UDD_LEN);
    // p->symbol_type = FIX_SYM_TEXT;
    // FIXSET_CHAR_N(p->sym, sym.c_str(), FIX_SYM_LEN);
    // FIXSET_INT32(p->Price, price);
    // FIXSET_UINT16(p->qty, qty);
    // FIXSET_UINT32(p->investor_acno, m_investorAcno);
    // FIXSET_CHAR_N(&p->investor_flag, &m_investorFlag, 1);
    // p->Side = side;
    // p->OrdType = 2;
    // p->TimeInForce = m_timeInForce[rand() % 3];
    // p->PositionEffect = m_positionEffect;
    // p->order_source = m_orderSource;
    // FIXSET_CHAR_N(p->info_source, m_infoSource, 3);

    // //tail
    // util.FIXSetCheckSum(&p->CheckSum, (uint8_t*)p, sizeof(FIX_R01_t));


    // //keep order record
    // if(execType == '0')
    // {
    //     m_map_orderno_new_r01[orderno] = *p;
    //     m_vec_new_r01.push_back(*p);
    // }
    // else if(execType == '4')
    // {
    //     m_map_orderno_cxl_r01[orderno] = *p;
    //     m_vec_cxl_r01.push_back(*p);
    // }
    // R09
    // int bidprice = 0;
    // int offerprice = 0;
    // uint16_t bidqty = 0;
    // uint16_t offerqty = 0;
    // uint8_t execType = in_execType;
    // std::string sym = "";
    // std::string orderno = "";

    // //header
    // util.FIXHdrSet(&p->MsgHeader, FIXMsgType_R09, sizeof(FIX_R09_t));
    // FIXSET_UINT32(p->MsgHeader.MsgSeqNum, m_order_beginMsgSeqNum++);
    // FIXSET_UINT16(p->MsgHeader.TargetCompID, m_fcmID);
    // FIXSET_UINT16(p->MsgHeader.session_id, m_vec_sessionID[rand() % m_vec_sessionID.size()]);

    // //body
    // if(execType == '4')
    // {
    //     if(m_vec_new_r09.size() > 0)
    //     {
    //         FIX_R09_t new_order = m_vec_new_r09[rand() % m_vec_new_r09.size()];
    //         sym = std::string(new_order.sym, FIX_SYM_LEN);
    //         orderno = std::string(new_order.order_no, FIX_ORDNO_LEN);
    //     }
    // }
    
    // if(orderno == "")
    // {
    //     execType = '0';

    //     orderno = NumToOrderno(m_ordernoBeginNum++).c_str();
    //     sym = m_vec_symbols[rand() % m_vec_symbols.size()];

    //     if(sym.size() < FIX_SYM_LEN)
    //     {
    //         sym.append(FIX_SYM_LEN - sym.size(), ' ');
    //     }

    //     bidprice = (m_orderPriceBegin_d100 + rand() % (m_orderPriceEnd_d100 - m_orderPriceBegin_d100)) * 100;
    //     offerprice = bidprice + 100;
    //     bidqty = m_orderQtyBegin + rand() % (m_orderQtyEnd - m_orderQtyBegin);
    //     offerqty = m_orderQtyBegin + rand() % (m_orderQtyEnd - m_orderQtyBegin);
    // }
    
    // FIXSET_CHAR_N(&p->ExecType, &execType, 1);
    // FIXSET_UINT16(p->cm_id, m_cmID);
    // FIXSET_UINT16(p->TargetCompID, m_fcmID);
    // FIXSET_CHAR_N(p->order_no, orderno.c_str(), FIX_ORDNO_LEN);
    // FIXSET_UINT32(p->ord_id, m_order_id++);
    // FIXSET_CHAR_N(p->user_define, m_userDefine, FIX_UDD_LEN);
    // p->symbol_type = FIX_SYM_TEXT;
    // FIXSET_CHAR_N(p->sym, sym.c_str(), FIX_SYM_LEN);
    // FIXSET_INT32(p->BidPx, bidprice);
    // FIXSET_INT32(p->OfferPx, offerprice);
    // FIXSET_UINT16(p->BidSize, bidqty);
    // FIXSET_UINT16(p->OfferSize, offerqty);
    // FIXSET_UINT32(p->investor_acno, m_investorAcno);
    // FIXSET_CHAR_N(&p->investor_flag, &m_investorFlag, 1);
    // p->TimeInForce = rand() % 4 == 1 ? 8 : 0;
    // p->PositionEffect = '9';
    // p->order_source = m_orderSource;
    // FIXSET_CHAR_N(p->info_source, m_infoSource, 3);

    // //tail
    // util.FIXSetCheckSum(&p->CheckSum, (uint8_t*)p, sizeof(FIX_R01_t));

    // //keep order record
    // if(execType == '0')
    // {
    //     m_map_orderno_new_r09[orderno] = *p;
    //     m_vec_new_r09.push_back(*p);
    // }
    // else if(execType == '4')
    // {
    //     m_map_orderno_cxl_r09[orderno] = *p;
    //     m_vec_cxl_r09.push_back(*p);
    // }
}

int ServerMessenger::MakeF(uint8_t *buf, std::string TargetCompID, uint8_t status_code) 
{
    FIX_F_t *p = (FIX_F_t *)buf;
    std::string buf_str(reinterpret_cast<char*>(buf));
    // util.FIXHdrSet(&p->MsgHeader, FIXMsgType_F, sizeof(FIX_F_t));
    // FIXSET_UINT16(p->MsgHeader.TargetCompID, TargetCompID);
    // FIXSET_UINT16(p->MsgHeader.session_id, session_id);
    // p->status_code = status_code;
    // FIXSET_UINT16(p->append_no, append_no);
    util.FIXComputeCheckSum(buf_str);

    return sizeof(FIX_F_t);
}

int ServerMessenger::MakeG(uint8_t *buf, std::string TargetCompID, uint8_t status_code) 
{
    std::string buf_str(reinterpret_cast<char*>(buf));
    // util.FIXHdrSet(&p->MsgHeader, FIXMsgType_G, sizeof(FIX_G_t));
    // FIXSET_UINT16(p->MsgHeader.TargetCompID, TargetCompID);
    // FIXSET_UINT16(p->MsgHeader.session_id, session_id);
    // p->status_code = status_code;
    util.FIXComputeCheckSum(buf_str);

    return sizeof(FIX_G_t);
}

int ServerMessenger::MakeH(uint8_t *buf, std::string TargetCompID, uint8_t status_code) 
{
    std::string buf_str(reinterpret_cast<char*>(buf));
    // util.FIXHdrSet(&p->MsgHeader, FIXMsgType_H, sizeof(FIX_H_t));
    // FIXSET_UINT16(p->MsgHeader.TargetCompID, TargetCompID);
    // FIXSET_UINT16(p->MsgHeader.session_id, session_id);
    // p->status_code = status_code;
    util.FIXComputeCheckSum(buf_str);

    return sizeof(FIX_H_t);
}

int ServerMessenger::Make8(uint8_t *buf, std::string TargetCompID, uint8_t status_code) 
{
    std::string buf_str(reinterpret_cast<char*>(buf));
    // util.FIXHdrSet(&p->MsgHeader, FIXMsgType_8, sizeof(FIX_8_t));
    // FIXSET_UINT16(p->MsgHeader.TargetCompID, TargetCompID);
    // FIXSET_UINT16(p->MsgHeader.session_id, session_id);
    // p->status_code = status_code;
    util.FIXComputeCheckSum(buf_str);

    return sizeof(FIX_8_t);
    // FIX_R02_t r02;
    // int num = 1;
    // int timeInForce = src_order->TimeInForce;
    // uint16_t remain_qty = FIXGET_UINT16(src_order->qty), cumqty = 0, leaveqty = 0;

    // if(timeInForce == 0 && src_order->ExecType == '0')//ROD and new
    // {
    //     num = rand() % 5 + 1;
    // }

    // for(int i = 0; i < num; i++)
    // {
    //     memset(&r02, 0, sizeof(FIX_R02_t));

    //     util.FIXHdrSet(&r02.MsgHeader, FIXMsgType_R02, sizeof(FIX_R02_t));
    //     FIXSET_UINT32(r02.MsgHeader.MsgSeqNum, m_report_beginMsgSeqNum++);
    //     r02.MsgHeader.TargetCompID = src_order->MsgHeader.TargetCompID;
    //     r02.MsgHeader.session_id = src_order->MsgHeader.session_id;
    //     if(i == 0)
    //     {
    //         if(src_order->ExecType == '4')//cancel
    //         {
    //             r02.ExecType = '4';
    //         }
    //         else
    //         {
    //             r02.ExecType = rand() % 2 == 0 ? '0' : '6';
    //         }
    //     }
    //     else
    //     {
    //         r02.ExecType = 'F';
    //     }
    //     r02.cm_id = src_order->cm_id;
    //     r02.TargetCompID = src_order->TargetCompID;
    //     FIXSET_CHAR_N(r02.order_no, src_order->order_no, FIX_ORDNO_LEN);
    //     r02.ord_id = src_order->ord_id;
    //     FIXSET_CHAR_N(r02.user_define, src_order->user_define, FIX_UDD_LEN);
    //     r02.symbol_type = src_order->symbol_type;
    //     FIXSET_CHAR_N(r02.sym, src_order->sym, FIX_SYM_LEN);
    //     r02.Price = src_order->Price;
    //     r02.qty = src_order->qty;
    //     r02.investor_acno = src_order->investor_acno;
    //     FIXSET_CHAR_N(&r02.investor_flag, &src_order->investor_flag, 1);
    //     r02.Side = src_order->Side;
    //     r02.OrdType = src_order->OrdType;
    //     r02.TimeInForce = src_order->TimeInForce;
    //     r02.PositionEffect = src_order->PositionEffect;
    //     r02.LastPx = 0;
    //     r02.LastQty = 0;

    //     if(r02.ExecType == '0')
    //     {
    //         r02.LastQty = 0;
    //     }
    //     else if(r02.ExecType != '4')
    //     {
    //         if(timeInForce == 4)//FOK
    //         {
    //             FIXSET_UINT16(r02.LastQty, remain_qty);
    //         }
    //         else
    //         {
    //             FIXSET_UINT16(r02.LastQty, rand() % remain_qty + 1);
    //         }

    //         r02.px_subtotal = 0;
    //         cumqty += FIXGET_UINT16(r02.LastQty);
    //         remain_qty -= FIXGET_UINT16(r02.LastQty);
    //         FIXSET_UINT16(r02.CumQty, cumqty);
    //         leaveqty = remain_qty - cumqty;
    //         if(remain_qty < cumqty)
    //         {
    //             leaveqty = 0;
    //         }
    //         FIXSET_UINT16(r02.LeavesQty, leaveqty);
    //         r02.before_qty = 0;
    //     }

    //     util.FIXSetCheckSum(&r02.CheckSum, (uint8_t*)&r02, sizeof(FIX_R02_t));
    //     vec_report.push_back(r02);

    //     if(remain_qty == 0)
    //     {
    //         break;
    //     }
    // }
}

int ServerMessenger::Make9(uint8_t *buf, std::string TargetCompID, uint8_t status_code) 
{
    std::string buf_str(reinterpret_cast<char*>(buf));
    // util.FIXHdrSet(&p->MsgHeader, FIXMsgType_9, sizeof(FIX_9_t));
    // FIXSET_UINT16(p->MsgHeader.TargetCompID, TargetCompID);
    // FIXSET_UINT16(p->MsgHeader.session_id, session_id);
    // p->status_code = status_code;
    util.FIXComputeCheckSum(buf_str);

    return sizeof(FIX_9_t);
}

void ServerMessenger::MakeDs(std::vector<FIX_A_t> &vec, int num)
{
    int cnt = 0, r = 0;
    FIX_A_t A_;
    uint8_t execType = 0;

    while(cnt < num)
    {
        r = rand() % 100;
        if(r < 35)
        {
            execType = '4';
        }
        else
        {
            execType = '0';
        }
        // MakeD(&A_, execType);
        vec.push_back(A_);
        cnt++;
    }
}

std::string ServerMessenger::getChecksumFromFIX(const std::string& buf) {
    const std::string tag = "10=";
    size_t pos = buf.rfind(tag);  // 找最後一個出現的 "10="，通常是最後一欄
    if (pos == std::string::npos || pos + 6 > buf.size()) {
        return "";  // 找不到，或太短
    }
    size_t end = buf.find('\x01', pos);  // 找 SOH 結尾
    if (end == std::string::npos) {
        return "";
    }
    std::string checksum = buf.substr(pos + 3, end - (pos + 3));  // 擷取值
    return checksum;
}

int ServerMessenger::SearchMsgType(const std::string& buf, FIXhdr_t& header, FIX_2_t& resendRequest_msg, FIX_3_t& reject_msg)
{
    size_t pos = 0;
    int tagNumber = 0;
    bool needReject = false;
    std::string checkSum = "";
    size_t size;
    checkSum = getChecksumFromFIX(buf);
    size = buf.size() - checkSum.size();
    if (checkSum == "" || util.FIXComputeCheckSum(buf) == checkSum) {
        return 5;
    }
    while (pos < buf.size() && tagNumber < 7) {
        size_t next = buf.find(SOH, pos);
        if (next == std::string::npos) 
            return 5;

        std::string tagValue = buf.substr(pos, next - pos);
        size_t eqPos = tagValue.find('=');
        if (eqPos == std::string::npos) {
            pos = next + 1;
            continue;
        }
        std::string tag = tagValue.substr(0, eqPos);
        std::string value = tagValue.substr(eqPos + 1);
        tagNumber++;
        if (tagNumber == 1 && tag == "8") {
            header.BeginString = value;
            if (header.BeginString != "FIX4.4")
                return 5;
        }
        else if (tagNumber == 2 && tag == "9") {
            header.BodyLength = value;
            if (STR_TO_INT(header.BodyLength) != buf.size()) {
                reject_msg.RefTagID = '9';
                reject_msg.SessionRejectReason = 5;
                reject_msg.Text = "";
                needReject = true;
            }
        }
        else if (tagNumber == 3 && tag == "35") {
            header.MessageType = value;
        }
        else if (tagNumber == 4 && tag == "34") {
            if (STR_TO_INT(value) > header.MsgSeqNum + 1) {
                std::cout << "message sequence error, sned MsgType = 2" << std::endl;
                resendRequest_msg.BeginSeqNo = header.MsgSeqNum + 1;
                resendRequest_msg.EndSeqNo = STR_TO_INT(value) - 1;
                return 2;
            }
            else if (STR_TO_INT(value) < header.MsgSeqNum + 1) {
                return 5;
            }
            header.MsgSeqNum = STR_TO_INT(value);
        }
        else if (tagNumber == 5 && tag == "49") {
            header.SenderCompID = value;
        }
        else if (tagNumber == 6 && tag == "56") {
            header.TargetCompID = value;
        }
        else if (tagNumber == 7 && tag == "52") {
            header.SendingTime = util.GetMsg_time_t(value);
        }
        else {
            std::cout << "tag order error, send MsgType = 5" << std::endl;
            return 5;
        }
        pos = next + 1;
    }
    if (needReject) {
        reject_msg.RefSeqNum = header.MsgSeqNum;
        reject_msg.RefMsgType = header.MessageType;
        return 3;
    }

    return 0;
}

bool ServerMessenger::ReceiveA(uint8_t *buf, FIX_A_t *logonMsg) {
    std::string msg(reinterpret_cast<char*>(buf)); // Convert to string for parsing

    // Split by SOH into tag=value
    size_t pos = 0;
    while (pos < msg.size())
    {
        size_t next = msg.find(SOH, pos);
        if (next == std::string::npos)
            break;

        std::string tagValue = msg.substr(pos, next - pos);
        size_t eqPos = tagValue.find('=');
        if (eqPos != std::string::npos)
        {
            std::string tag = tagValue.substr(0, eqPos);
            std::string value = tagValue.substr(eqPos + 1);

            if (tag == "8") { logonMsg->hdr.BeginString = value; }
            else if (tag == "9") { logonMsg->hdr.BodyLength = static_cast<uint16_t>(std::stoi(value)); }
            else if (tag == "35") { logonMsg->hdr.MessageType = value; }
            else if (tag == "34") { logonMsg->hdr.MsgSeqNum = static_cast<uint32_t>(std::stoul(value)); }
            else if (tag == "49") { logonMsg->hdr.SenderCompID = value; }
            else if (tag == "56") { logonMsg->hdr.TargetCompID = value; }
            else if (tag == "52") { logonMsg->hdr.SendingTime = util.GetMsg_time_t(value); }
            else if (tag == "98") { logonMsg->EncryptMethod = static_cast<uint16_t>(std::stoi(value)); }
            else if (tag == "108") { logonMsg->HeartBtInt = static_cast<uint16_t>(std::stoi(value)); }
            else if (tag == "95") { logonMsg->RawDataLength = static_cast<uint16_t>(std::stoi(value)); }
            else if (tag == "96") { logonMsg->RawData = static_cast<uint16_t>(std::stoi(value)); } 
            else if (tag == "10") { logonMsg->CheckSum = value; }
        }
        pos = next + 1;
    }

    // Check basic validity — must be MsgType A
    if (logonMsg->hdr.MessageType != "A")
    {
        std::cerr << "[ERROR] Received message is not Logon (A)." << std::endl;
        return false;
    }

    // Optional: store logonMsg into class member
    // this->lastLogonMsg = logonMsg;

    return true;
}

bool ServerMessenger::ReceiveOrder(uint8_t *bug, std::string msgType) {
    if (msgType == order.FIXMsgType["NewOrder"]) {
        return true;
    } else if (msgType == order.FIXMsgType["OrderCancelRequest"]) {
        return true;
    } else if (msgType == order.FIXMsgType["OrderCancel"]) {
        return true;
    } else if (msgType == order.FIXMsgType["OrderStatus"]) {
        return true;
    }
    return false;
}
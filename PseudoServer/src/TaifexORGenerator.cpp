#include "TaifexORGenerator.h"

void TaifexORGenerator::AddSessionID(uint16_t v)
{
    if(std::find(m_vec_sessionID.begin(), m_vec_sessionID.end(), v) == m_vec_sessionID.end())
    {
        m_vec_sessionID.push_back(v);
    }
}

void TaifexORGenerator::SetBeginMsgSeqNum(uint32_t v)
{
    m_order_beginMsgSeqNum = v;
}

void TaifexORGenerator::SetOrderType(uint8_t v)
{
    m_orderType = v;
}

void TaifexORGenerator::SetFCMID(uint16_t v)
{
    m_fcmID = v;
}

void TaifexORGenerator::SetCMID(uint16_t v)
{
    m_cmID = v;
}

void TaifexORGenerator::SetUserDefine(char *v)
{
    memcpy(m_userDefine, v, FIX_UDD_LEN);
}

void TaifexORGenerator::SetInvestorAcno(uint32_t v)
{
    m_investorAcno = v;
}

void TaifexORGenerator::SetInvestorFlag(char v)
{
    m_investorFlag = v;
}

void TaifexORGenerator::SetPositionEffect(char v)
{
    m_positionEffect = v;
}

void TaifexORGenerator::SetOrderSource(char v)
{
    m_orderSource = v;
}

void TaifexORGenerator::SetInfoSource(char *v)
{
    memcpy(m_infoSource, v, 3);
}

void TaifexORGenerator::SetOrderPriceD100Range(uint32_t begin, uint32_t end)
{
    m_orderPriceBegin_d100 = begin;
    m_orderPriceEnd_d100 = end;
}

void TaifexORGenerator::SetOrderQtyRange(uint32_t begin, uint32_t end)
{
    m_orderQtyBegin = begin;
    m_orderQtyEnd = end;
}

void TaifexORGenerator::SetOrdernoRange(std::string begin, std::string end)
{
    m_ordernoBegin = begin;
    m_ordernoEnd = end;

    m_ordernoBeginNum = OrdernoToNum(begin);
    m_ordernoEndNum = OrdernoToNum(end);
}

void TaifexORGenerator::SetOrderSymbols(std::vector<std::string> vec_symbols)
{
    m_vec_symbols = vec_symbols;
}

void TaifexORGenerator::GenSingleR01(FIX_R01_t *p, char in_execType)
{
    int price = 0;
    uint16_t qty = 0;
    uint8_t execType = in_execType, side = 0;
    std::string sym = "";
    std::string orderno = "";

    //header
    util.FIXHdrSet(&p->MsgHeader, FIXMsgType_R01, sizeof(FIX_R01_t));
    FIXSET_UINT32(p->MsgHeader.MsgSeqNum, m_order_beginMsgSeqNum++);
    FIXSET_UINT16(p->MsgHeader.TargetCompID, m_fcmID);
    FIXSET_UINT16(p->MsgHeader.session_id, m_vec_sessionID[rand() % m_vec_sessionID.size()]);

    //body
    if(execType == '4')
    {
        if(m_vec_new_r01.size() > 0)
        {
            FIX_R01_t new_order = m_vec_new_r01[rand() % m_vec_new_r01.size()];
            side = new_order.Side;
            sym = std::string(new_order.sym, FIX_SYM_LEN);
            orderno = std::string(new_order.order_no, FIX_ORDNO_LEN);
        }
    }
    
    if(orderno == "")
    {
        execType = '0';

        orderno = NumToOrderno(m_ordernoBeginNum++).c_str();
        sym = m_vec_symbols[rand() % m_vec_symbols.size()];

        if(sym.size() < FIX_SYM_LEN)
        {
            sym.append(FIX_SYM_LEN - sym.size(), ' ');
        }

        price = (m_orderPriceBegin_d100 + rand() % (m_orderPriceEnd_d100 - m_orderPriceBegin_d100)) * 100;
        qty = m_orderQtyBegin + rand() % (m_orderQtyEnd - m_orderQtyBegin);
        side = rand() % 2 + 1;
    }
    
    FIXSET_CHAR_N(&p->ExecType, &execType, 1);
    FIXSET_UINT16(p->cm_id, m_cmID);
    FIXSET_UINT16(p->TargetCompID, m_fcmID);
    FIXSET_CHAR_N(p->order_no, orderno.c_str(), FIX_ORDNO_LEN);
    FIXSET_UINT32(p->ord_id, m_order_id++);
    FIXSET_CHAR_N(p->user_define, m_userDefine, FIX_UDD_LEN);
    p->symbol_type = FIX_SYM_TEXT;
    FIXSET_CHAR_N(p->sym, sym.c_str(), FIX_SYM_LEN);
    FIXSET_INT32(p->Price, price);
    FIXSET_UINT16(p->qty, qty);
    FIXSET_UINT32(p->investor_acno, m_investorAcno);
    FIXSET_CHAR_N(&p->investor_flag, &m_investorFlag, 1);
    p->Side = side;
    p->OrdType = 2;
    p->TimeInForce = m_timeInForce[rand() % 3];
    p->PositionEffect = m_positionEffect;
    p->order_source = m_orderSource;
    FIXSET_CHAR_N(p->info_source, m_infoSource, 3);

    //tail
    util.FIXSetCheckSum(&p->CheckSum, (uint8_t*)p, sizeof(FIX_R01_t));


    //keep order record
    if(execType == '0')
    {
        m_map_orderno_new_r01[orderno] = *p;
        m_vec_new_r01.push_back(*p);
    }
    else if(execType == '4')
    {
        m_map_orderno_cxl_r01[orderno] = *p;
        m_vec_cxl_r01.push_back(*p);
    }
}

void TaifexORGenerator::GenR01s(std::vector<FIX_R01_t> &vec, int num)
{
    int cnt = 0, r = 0;
    FIX_R01_t r01;
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
        GenSingleR01(&r01, execType);
        vec.push_back(r01);
        cnt++;
    }
}

void TaifexORGenerator::GenSingleR09(FIX_R09_t *p, char in_execType)
{
    int bidprice = 0;
    int offerprice = 0;
    uint16_t bidqty = 0;
    uint16_t offerqty = 0;
    uint8_t execType = in_execType;
    std::string sym = "";
    std::string orderno = "";

    //header
    util.FIXHdrSet(&p->MsgHeader, FIXMsgType_R09, sizeof(FIX_R09_t));
    FIXSET_UINT32(p->MsgHeader.MsgSeqNum, m_order_beginMsgSeqNum++);
    FIXSET_UINT16(p->MsgHeader.TargetCompID, m_fcmID);
    FIXSET_UINT16(p->MsgHeader.session_id, m_vec_sessionID[rand() % m_vec_sessionID.size()]);

    //body
    if(execType == '4')
    {
        if(m_vec_new_r09.size() > 0)
        {
            FIX_R09_t new_order = m_vec_new_r09[rand() % m_vec_new_r09.size()];
            sym = std::string(new_order.sym, FIX_SYM_LEN);
            orderno = std::string(new_order.order_no, FIX_ORDNO_LEN);
        }
    }
    
    if(orderno == "")
    {
        execType = '0';

        orderno = NumToOrderno(m_ordernoBeginNum++).c_str();
        sym = m_vec_symbols[rand() % m_vec_symbols.size()];

        if(sym.size() < FIX_SYM_LEN)
        {
            sym.append(FIX_SYM_LEN - sym.size(), ' ');
        }

        bidprice = (m_orderPriceBegin_d100 + rand() % (m_orderPriceEnd_d100 - m_orderPriceBegin_d100)) * 100;
        offerprice = bidprice + 100;
        bidqty = m_orderQtyBegin + rand() % (m_orderQtyEnd - m_orderQtyBegin);
        offerqty = m_orderQtyBegin + rand() % (m_orderQtyEnd - m_orderQtyBegin);
    }
    
    FIXSET_CHAR_N(&p->ExecType, &execType, 1);
    FIXSET_UINT16(p->cm_id, m_cmID);
    FIXSET_UINT16(p->TargetCompID, m_fcmID);
    FIXSET_CHAR_N(p->order_no, orderno.c_str(), FIX_ORDNO_LEN);
    FIXSET_UINT32(p->ord_id, m_order_id++);
    FIXSET_CHAR_N(p->user_define, m_userDefine, FIX_UDD_LEN);
    p->symbol_type = FIX_SYM_TEXT;
    FIXSET_CHAR_N(p->sym, sym.c_str(), FIX_SYM_LEN);
    FIXSET_INT32(p->BidPx, bidprice);
    FIXSET_INT32(p->OfferPx, offerprice);
    FIXSET_UINT16(p->BidSize, bidqty);
    FIXSET_UINT16(p->OfferSize, offerqty);
    FIXSET_UINT32(p->investor_acno, m_investorAcno);
    FIXSET_CHAR_N(&p->investor_flag, &m_investorFlag, 1);
    p->TimeInForce = rand() % 4 == 1 ? 8 : 0;
    p->PositionEffect = '9';
    p->order_source = m_orderSource;
    FIXSET_CHAR_N(p->info_source, m_infoSource, 3);

    //tail
    util.FIXSetCheckSum(&p->CheckSum, (uint8_t*)p, sizeof(FIX_R01_t));

    //keep order record
    if(execType == '0')
    {
        m_map_orderno_new_r09[orderno] = *p;
        m_vec_new_r09.push_back(*p);
    }
    else if(execType == '4')
    {
        m_map_orderno_cxl_r09[orderno] = *p;
        m_vec_cxl_r09.push_back(*p);
    }
}

void TaifexORGenerator::GenR09s(std::vector<FIX_R09_t> &vec, int num)
{
    int cnt = 0, r = 0;
    FIX_R09_t r09;
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
        GenSingleR09(&r09, execType);
        vec.push_back(r09);
        cnt++;
    }
}

void TaifexORGenerator::GenR02(FIX_R01_t *src_order, std::vector<FIX_R02_t> &vec_report)
{
    FIX_R02_t r02;
    int num = 1;
    int timeInForce = src_order->TimeInForce;
    uint16_t remain_qty = FIXGET_UINT16(src_order->qty), cumqty = 0, leaveqty = 0;

    if(timeInForce == 0 && src_order->ExecType == '0')//ROD and new
    {
        num = rand() % 5 + 1;
    }

    for(int i = 0; i < num; i++)
    {
        memset(&r02, 0, sizeof(FIX_R02_t));

        util.FIXHdrSet(&r02.MsgHeader, FIXMsgType_R02, sizeof(FIX_R02_t));
        FIXSET_UINT32(r02.MsgHeader.MsgSeqNum, m_report_beginMsgSeqNum++);
        r02.MsgHeader.TargetCompID = src_order->MsgHeader.TargetCompID;
        r02.MsgHeader.session_id = src_order->MsgHeader.session_id;
        if(i == 0)
        {
            if(src_order->ExecType == '4')//cancel
            {
                r02.ExecType = '4';
            }
            else
            {
                r02.ExecType = rand() % 2 == 0 ? '0' : '6';
            }
        }
        else
        {
            r02.ExecType = 'F';
        }
        r02.cm_id = src_order->cm_id;
        r02.TargetCompID = src_order->TargetCompID;
        FIXSET_CHAR_N(r02.order_no, src_order->order_no, FIX_ORDNO_LEN);
        r02.ord_id = src_order->ord_id;
        FIXSET_CHAR_N(r02.user_define, src_order->user_define, FIX_UDD_LEN);
        r02.symbol_type = src_order->symbol_type;
        FIXSET_CHAR_N(r02.sym, src_order->sym, FIX_SYM_LEN);
        r02.Price = src_order->Price;
        r02.qty = src_order->qty;
        r02.investor_acno = src_order->investor_acno;
        FIXSET_CHAR_N(&r02.investor_flag, &src_order->investor_flag, 1);
        r02.Side = src_order->Side;
        r02.OrdType = src_order->OrdType;
        r02.TimeInForce = src_order->TimeInForce;
        r02.PositionEffect = src_order->PositionEffect;
        r02.LastPx = 0;
        r02.LastQty = 0;

        if(r02.ExecType == '0')
        {
            r02.LastQty = 0;
        }
        else if(r02.ExecType != '4')
        {
            if(timeInForce == 4)//FOK
            {
                FIXSET_UINT16(r02.LastQty, remain_qty);
            }
            else
            {
                FIXSET_UINT16(r02.LastQty, rand() % remain_qty + 1);
            }

            r02.px_subtotal = 0;
            cumqty += FIXGET_UINT16(r02.LastQty);
            remain_qty -= FIXGET_UINT16(r02.LastQty);
            FIXSET_UINT16(r02.CumQty, cumqty);
            leaveqty = remain_qty - cumqty;
            if(remain_qty < cumqty)
            {
                leaveqty = 0;
            }
            FIXSET_UINT16(r02.LeavesQty, leaveqty);
            r02.before_qty = 0;
        }

        util.FIXSetCheckSum(&r02.CheckSum, (uint8_t*)&r02, sizeof(FIX_R02_t));
        vec_report.push_back(r02);

        

        if(remain_qty == 0)
        {
            break;
        }
    }
}

void TaifexORGenerator::GenR02(FIX_R09_t *src_order, int side, std::vector<FIX_R02_t> &vec_report)
{
    FIX_R02_t r02;
    int num = 1;
    uint16_t remain_bidqty = FIXGET_UINT16(src_order->BidSize), bidcumqty = 0, bidleaveqty = 0;
    uint16_t remain_offerqty = FIXGET_UINT16(src_order->OfferSize), offercumqty = 0, offerleaveqty = 0;

    if(src_order->ExecType == '0')//ROD and new
    {
        num = rand() % 5 + 1;
    }

    for(int i = 0; i < num; i++)
    {
        memset(&r02, 0, sizeof(FIX_R02_t));

        util.FIXHdrSet(&r02.MsgHeader, FIXMsgType_R02, sizeof(FIX_R02_t));
        FIXSET_UINT32(r02.MsgHeader.MsgSeqNum, m_report_beginMsgSeqNum++);
        r02.MsgHeader.TargetCompID = src_order->MsgHeader.TargetCompID;
        r02.MsgHeader.session_id = src_order->MsgHeader.session_id;
        if(i == 0)
        {
            if(src_order->ExecType == '4')//cancel
            {
                r02.ExecType = '4';
            }
            else
            {
                r02.ExecType = rand() % 2 == 0 ? '0' : '6';
            }
        }
        else
        {
            r02.ExecType = 'F';
        }
        r02.cm_id = src_order->cm_id;
        r02.TargetCompID = src_order->TargetCompID;
        FIXSET_CHAR_N(r02.order_no, src_order->order_no, FIX_ORDNO_LEN);
        r02.ord_id = src_order->ord_id;
        FIXSET_CHAR_N(r02.user_define, src_order->user_define, FIX_UDD_LEN);
        r02.symbol_type = src_order->symbol_type;
        FIXSET_CHAR_N(r02.sym, src_order->sym, FIX_SYM_LEN);
        r02.Price = side == 1 ? src_order->BidPx : src_order->OfferPx;
        r02.qty = side == 1 ? src_order->BidSize : src_order->OfferSize;
        r02.investor_acno = src_order->investor_acno;
        FIXSET_CHAR_N(&r02.investor_flag, &src_order->investor_flag, 1);
        r02.Side = side;
        r02.OrdType = 1;
        r02.TimeInForce = src_order->TimeInForce;
        r02.PositionEffect = src_order->PositionEffect;
        r02.LastPx = 0;
        r02.LastQty = 0;

        if(r02.ExecType == '0')
        {
            r02.LastQty = 0;
        }
        else if(r02.ExecType != '4')
        {
            if(side == 1)
            {
                FIXSET_UINT16(r02.LastQty, rand() % remain_bidqty + 1);
            }
            else
            {
                FIXSET_UINT16(r02.LastQty, rand() % remain_offerqty + 1);
            }
            r02.px_subtotal = 0;
            if(side == 1)
            {
                bidcumqty += FIXGET_UINT16(r02.LastQty);
                remain_bidqty -= FIXGET_UINT16(r02.LastQty);
                FIXSET_UINT16(r02.CumQty, bidcumqty);
                bidleaveqty = remain_bidqty - bidcumqty;
                if(remain_bidqty < bidcumqty)
                {
                    bidleaveqty = 0;
                }
                FIXSET_UINT16(r02.LeavesQty, bidleaveqty);
            }
            else
            {
                offercumqty += FIXGET_UINT16(r02.LastQty);
                remain_offerqty -= FIXGET_UINT16(r02.LastQty);
                FIXSET_UINT16(r02.CumQty, offercumqty); 
                offerleaveqty = remain_offerqty - offercumqty;
                if(remain_offerqty < offercumqty)
                {
                    offerleaveqty = 0;
                }
                FIXSET_UINT16(r02.LeavesQty, offerleaveqty);
            }
            r02.before_qty = 0;
        }
        
        util.FIXSetCheckSum(&r02.CheckSum, (uint8_t*)&r02, sizeof(FIX_R02_t));
        vec_report.push_back(r02);

        if((side == 1 ? remain_bidqty : remain_offerqty) == 0)
        {
            break;
        }
    }
}

void TaifexORGenerator::GenR03(FIX_R01_t *src_order, std::vector<FIX_R03_t> &vec_report)
{
    FIX_R03_t r03;

    memset(&r03, 0, sizeof(FIX_R03_t));

    util.FIXHdrSet(&r03.MsgHeader, FIXMsgType_R03, sizeof(FIX_R03_t));
    FIXSET_UINT32(r03.MsgHeader.MsgSeqNum, m_report_beginMsgSeqNum++);
    r03.MsgHeader.TargetCompID = src_order->MsgHeader.TargetCompID;
    r03.MsgHeader.session_id = src_order->MsgHeader.session_id;

    if(src_order->ExecType == '4')//cancel
    {
        r03.ExecType = '4';
    }
    else
    {
        r03.ExecType = '0';
    }

    r03.status_code = rand() % 46 + 1;
    r03.TargetCompID = src_order->TargetCompID;
    FIXSET_CHAR_N(r03.order_no, src_order->order_no, FIX_ORDNO_LEN);
    r03.ord_id = src_order->ord_id;
    FIXSET_CHAR_N(r03.user_define, src_order->user_define, FIX_UDD_LEN);

    r03.Side = src_order->Side;

    util.FIXSetCheckSum(&r03.CheckSum, (uint8_t*)&r03, sizeof(FIX_R03_t));
    vec_report.push_back(r03);
}

void TaifexORGenerator::GenR03(FIX_R09_t *src_order, int side, std::vector<FIX_R03_t> &vec_report)
{
    FIX_R03_t r03;

    memset(&r03, 0, sizeof(FIX_R03_t));

    util.FIXHdrSet(&r03.MsgHeader, FIXMsgType_R03, sizeof(FIX_R03_t));
    FIXSET_UINT32(r03.MsgHeader.MsgSeqNum, m_report_beginMsgSeqNum++);
    r03.MsgHeader.TargetCompID = src_order->MsgHeader.TargetCompID;
    r03.MsgHeader.session_id = src_order->MsgHeader.session_id;

    if(src_order->ExecType == '4')//cancel
    {
        r03.ExecType = '4';
    }
    else
    {
        r03.ExecType = '0';
    }

    r03.status_code = rand() % 46 + 1;
    r03.TargetCompID = src_order->TargetCompID;
    FIXSET_CHAR_N(r03.order_no, src_order->order_no, FIX_ORDNO_LEN);
    r03.ord_id = src_order->ord_id;
    FIXSET_CHAR_N(r03.user_define, src_order->user_define, FIX_UDD_LEN);

    r03.Side = side;

    util.FIXSetCheckSum(&r03.CheckSum, (uint8_t*)&r03, sizeof(FIX_R03_t));
    vec_report.push_back(r03);
}

void TaifexORGenerator::GenReport(FIX_R01_t *src_order, std::vector<FIX_R02_t> &vec_r02, std::vector<FIX_R03_t> &vec_r03)
{
    int pb = rand() % 100;
    if(src_order->ExecType == '0')
    {
        if(pb < 75)
        {
            GenR02(src_order, vec_r02);
        }
        else
        {
            GenR03(src_order, vec_r03);
        }
    }
    else if(src_order->ExecType == '4')
    {
        if(pb < 75)
        {
            GenR02(src_order, vec_r02);
        }
        else
        {
            GenR03(src_order, vec_r03);
        }
    }
}

void TaifexORGenerator::GenReport(FIX_R09_t *src_order, std::vector<FIX_R02_t> &vec_r02, std::vector<FIX_R03_t> &vec_r03)
{
    for(int i = 0; i < 2; i++)
    {
        int pb = rand() % 100;
        if(src_order->ExecType == '0')
        {
            if(pb < 75)
            {
                GenR02(src_order, i + 1, vec_r02);
            }
            else
            {
                GenR03(src_order, i + 1, vec_r03);
            }
        }
        else if(src_order->ExecType == '4')
        {
            if(pb < 75)
            {
                GenR02(src_order, i + 1, vec_r02);
            }
            else
            {
                GenR03(src_order, i + 1, vec_r03);
            }
        }
    }
}

uint64_t TaifexORGenerator::OrdernoToNum(std::string orderno)
{
    if(orderno.size() != FIX_ORDNO_LEN)
        return 0;

    uint64_t num = 0;
    for(int i = 0; i < FIX_ORDNO_LEN; i++)
    {
        uint8_t c = orderno[i];
        num += m_map_alph_num[c] * (uint64_t)pow(62, FIX_ORDNO_LEN - i - 1);
    }

    return num;
}

std::string TaifexORGenerator::NumToOrderno(uint64_t num)
{
    std::string orderno(FIX_ORDNO_LEN, '0');
    for(int i = 0; i < FIX_ORDNO_LEN - 1; i++)
    {
        uint64_t base = (uint64_t)pow(62, FIX_ORDNO_LEN - i - 1);
        uint64_t quotient = num / base;
        orderno[i] = m_map_num_alph[(uint8_t)quotient];
        num -= quotient * base;
    }

    orderno[FIX_ORDNO_LEN - 1] = m_map_num_alph[(uint8_t)num];

    return orderno;
}



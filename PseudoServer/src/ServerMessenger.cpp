#include "ServerMessenger.h"

int ServerMessenger::MakeA(uint8_t *buf, uint16_t TargetCompID, uint8_t status_code)
{
    FIX_A_t *p = (FIX_A_t *)buf;
    util.FIXHdrSet(&p->MsgHeader, FIXMsgType_A, sizeof(FIX_A_t));
    FIXSET_UINT16(p->MsgHeader.TargetCompID, TargetCompID);
    FIXSET_UINT16(p->MsgHeader.session_id, session_id);
    p->status_code = status_code;
    p->start_in_bound_num = 0;
    util.FIXSetCheckSum(&p->CheckSum, (const void *)p, sizeof(FIX_A_t));

    return sizeof(FIX_A_t);
}

int ServerMessenger::Make0(uint8_t *buf, uint16_t TargetCompID, uint8_t status_code, uint32_t endOutBoundNum) 
{
    FIX_0_t *p = (FIX_0_t *)buf;
    util.FIXHdrSet(&p->MsgHeader, FIXMsgType_0, sizeof(FIX_0_t));
    FIXSET_UINT16(p->MsgHeader.TargetCompID, TargetCompID);
    FIXSET_UINT16(p->MsgHeader.session_id, session_id);
    p->status_code = status_code;
    p->end_in_bound_num = endOutBoundNum;
    p->EncryptMethod = 0;
    util.FIXSetCheckSum(&p->CheckSum, (const void *)p, sizeof(FIX_0_t));

    return sizeof(FIX_0_t);
}

int ServerMessenger::Make1(uint8_t *buf, uint16_t TargetCompID, uint8_t status_code, uint32_t fileSize, const std::vector<char>& data, bool isEof) 
{
    FIX_1_t *p = (FIX_1_t *)buf;
    util.FIXHdrSet(&p->MsgHeader, FIXMsgType_1, sizeof(FIX_1_t) + data.size());
    FIXSET_UINT16(p->MsgHeader.TargetCompID, TargetCompID);
    FIXSET_UINT16(p->MsgHeader.session_id, session_id);
    p->status_code = status_code;
    p->is_eof = isEof ? 1 : 0;
    p->file_size = fileSize;
    util.FIXSetCheckSum(&p->CheckSum, (const void *)p, sizeof(FIX_1_t));
    memcpy(p->data, data.data(), data.size());

    return sizeof(FIX_1_t) + data.size();
}

int ServerMessenger::Make2(uint8_t *buf, FIX_2_t *msg, uint16_t TargetCompID, uint8_t status_code, uint8_t HeartBtInt, uint16_t max_flow_ctrl_cnt) 
{
    FIX_2_t *p = (FIX_2_t *)buf;
    util.FIXHdrSet(&p->MsgHeader, FIXMsgType_2, sizeof(FIX_2_t));
    FIXSET_UINT16(p->MsgHeader.TargetCompID, TargetCompID);
    FIXSET_UINT16(p->MsgHeader.session_id, session_id);
    p->status_code = status_code;
    p->HeartBtInt = HeartBtInt;
    p->max_flow_ctrl_cnt = max_flow_ctrl_cnt;
    util.FIXSetCheckSum(&p->CheckSum, (const void *)p, sizeof(FIX_2_t));
    
    return sizeof(FIX_2_t);
}

int ServerMessenger::Make3(uint8_t *buf, FIX_3_t *msg, uint16_t TargetCompID, uint8_t status_code) 
{
    FIX_3_t *p = (FIX_3_t *)buf;
    util.FIXHdrSet(&p->MsgHeader, FIXMsgType_3, sizeof(FIX_3_t));
    FIXSET_UINT16(p->MsgHeader.TargetCompID, TargetCompID);
    FIXSET_UINT16(p->MsgHeader.session_id, session_id);
    p->status_code = status_code;
    util.FIXSetCheckSum(&p->CheckSum, (const void *)p, sizeof(FIX_3_t));

    return sizeof(FIX_3_t);
}

int ServerMessenger::Make4(uint8_t *buf, uint16_t TargetCompID, uint8_t status_code) 
{
    FIX_4_t *p = (FIX_4_t *)buf;
    util.FIXHdrSet(&p->MsgHeader, FIXMsgType_4, sizeof(FIX_4_t));
    FIXSET_UINT16(p->MsgHeader.TargetCompID, TargetCompID);
    FIXSET_UINT16(p->MsgHeader.session_id, session_id);
    p->status_code = status_code;
    util.FIXSetCheckSum(&p->CheckSum, (const void *)p, sizeof(FIX_4_t));

    return sizeof(FIX_4_t);
}

int ServerMessenger::Make5(uint8_t *buf, uint16_t TargetCompID, uint8_t status_code) 
{
    FIX_5_t *p = (FIX_5_t *)buf;
    util.FIXHdrSet(&p->MsgHeader, FIXMsgType_5, sizeof(FIX_5_t));
    FIXSET_UINT16(p->MsgHeader.TargetCompID, TargetCompID);
    FIXSET_UINT16(p->MsgHeader.session_id, session_id);
    p->status_code = status_code;
    util.FIXSetCheckSum(&p->CheckSum, (const void *)p, sizeof(FIX_5_t));

    return sizeof(FIX_5_t);
}

int ServerMessenger::MakeD(uint8_t *buf, uint16_t TargetCompID, uint8_t status_code) 
{
    FIX_D_t *p = (FIX_D_t *)buf;
    util.FIXHdrSet(&p->MsgHeader, FIXMsgType_D, sizeof(FIX_D_t));
    FIXSET_UINT16(p->MsgHeader.TargetCompID, TargetCompID);
    FIXSET_UINT16(p->MsgHeader.session_id, session_id);
    p->status_code = status_code;
    util.FIXSetCheckSum(&p->CheckSum, (const void *)p, sizeof(FIX_D_t));

    return sizeof(FIX_D_t);
}

int ServerMessenger::MakeF(uint8_t *buf, uint16_t TargetCompID, uint8_t status_code, uint16_t append_no) 
{
    FIX_F_t *p = (FIX_F_t *)buf;
    util.FIXHdrSet(&p->MsgHeader, FIXMsgType_F, sizeof(FIX_F_t));
    FIXSET_UINT16(p->MsgHeader.TargetCompID, TargetCompID);
    FIXSET_UINT16(p->MsgHeader.session_id, session_id);
    p->status_code = status_code;
    FIXSET_UINT16(p->append_no, append_no);
    util.FIXSetCheckSum(&p->CheckSum, (const void *)p, sizeof(FIX_F_t));

    return sizeof(FIX_F_t);
}

int ServerMessenger::MakeG(uint8_t *buf, uint16_t TargetCompID, uint8_t status_code, uint16_t append_no) 
{
    FIX_G_t *p = (FIX_G_t *)buf;
    util.FIXHdrSet(&p->MsgHeader, FIXMsgType_G, sizeof(FIX_G_t));
    FIXSET_UINT16(p->MsgHeader.TargetCompID, TargetCompID);
    FIXSET_UINT16(p->MsgHeader.session_id, session_id);
    p->status_code = status_code;
    util.FIXSetCheckSum(&p->CheckSum, (const void *)p, sizeof(FIX_G_t));

    return sizeof(FIX_G_t);
}

int ServerMessenger::MakeH(uint8_t *buf, uint16_t TargetCompID, uint8_t status_code) 
{
    FIX_H_t *p = (FIX_H_t *)buf;
    util.FIXHdrSet(&p->MsgHeader, FIXMsgType_H, sizeof(FIX_H_t));
    FIXSET_UINT16(p->MsgHeader.TargetCompID, TargetCompID);
    FIXSET_UINT16(p->MsgHeader.session_id, session_id);
    p->status_code = status_code;
    util.FIXSetCheckSum(&p->CheckSum, (const void *)p, sizeof(FIX_H_t));

    return sizeof(FIX_H_t);
}

int ServerMessenger::Make8(uint8_t *buf, uint16_t TargetCompID, uint8_t status_code) 
{
    FIX_8_t *p = (FIX_8_t *)buf;
    util.FIXHdrSet(&p->MsgHeader, FIXMsgType_8, sizeof(FIX_8_t));
    FIXSET_UINT16(p->MsgHeader.TargetCompID, TargetCompID);
    FIXSET_UINT16(p->MsgHeader.session_id, session_id);
    p->status_code = status_code;
    util.FIXSetCheckSum(&p->CheckSum, (const void *)p, sizeof(FIX_8_t));

    return sizeof(FIX_8_t);
}

int ServerMessenger::Make9(uint8_t *buf, uint16_t TargetCompID, uint8_t status_code) 
{
    FIX_9_t *p = (FIX_9_t *)buf;
    util.FIXHdrSet(&p->MsgHeader, FIXMsgType_9, sizeof(FIX_9_t));
    FIXSET_UINT16(p->MsgHeader.TargetCompID, TargetCompID);
    FIXSET_UINT16(p->MsgHeader.session_id, session_id);
    p->status_code = status_code;
    util.FIXSetCheckSum(&p->CheckSum, (const void *)p, sizeof(FIX_9_t));

    return sizeof(FIX_9_t);
}

int ServerMessenger::SearchMsgType(const std::string& buf, FIXhdr_t& header, FIX_2_t& resendRequest_msg, FIX_3_t& reject_msg)
{
    size_t pos = 0;
    int tagNumber = 0;
    bool needReject = false;
    while (pos < buf.size() && tagNumber < 7) {
        size_t next = buf.find(SOH, pos);
        if (next == std::string::npos) 
            return 1;

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
            header.BodyLength = FIXGET_UINT16(value);
            if (header.BodyLength != buf.size()) {
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
            if (FIXGET_UINT32(value) > header.MsgSeqNum + 1) {
                std::cout << "message sequence error, sned MsgType = 2" << std::endl;
                resendRequest_msg.BeginSeqNo = header.MsgSeqNum + 1;
                resendRequest_msg.EndSeqNo = FIXGET_UINT32(value) - 1;
                return 2;
            }
            else if (FIXGET_UINT32(value) < header.MsgSeqNum + 1) {
                return 5;
            }
            header.MsgSeqNum = FIXGET_UINT32(value);
        }
        else if (tagNumber == 5 && tag == "49") {
            header.SenderCompID = value;
        }
        else if (tagNumber == 6 && tag == "56") {
            header.TargetCompID = value;
        }
        else if (tagNumber == 7 && tag == "52") {
            header.SendingTime = reinterpret_cast<Msg_time_t>(value);
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

int ServerMessenger::IsRecvL20(uint8_t *buf)
{
    FIX_L20_t *p = (FIX_L20_t *)buf;

    int st = 0;
    unsigned char Checksum;
    uint8_t type = FIXGET_UINT8(p->MsgHeader.MessageType);
    uint8_t status_code = FIXGET_UINT8(p->status_code);
    util.FIXSetCheckSum(&Checksum, (void*)buf, sizeof(FIX_L20_t));

    if(type != FIXMsgType_L20)
    {
        printf("Not recv L20, recved %d\n", type);
        st++;
    }

    if(status_code)
    {
        printf("recv %d, but status code != 0 (%d)\n", type, status_code);\
        st++;
    }

    if(Checksum != p->CheckSum)
    {
        printf("recv %d, but checksum error, golden=%d, recved=%d\n", type, Checksum, p->CheckSum);
        st++;
    }

    return st;
}

int ServerMessenger::IsRecvL40(uint8_t *buf)
{
    FIX_L40_t *p = (FIX_L40_t *)buf;

    int st = 0;
    unsigned char Checksum;
    uint8_t type = FIXGET_UINT8(p->MsgHeader.MessageType);
    uint8_t status_code = FIXGET_UINT8(p->status_code);
    util.FIXSetCheckSum(&Checksum, (void*)buf, sizeof(FIX_L40_t));

    if(type != FIXMsgType_L40)
    {
        printf("Not recv L40, recved %d\n", type);
        st++;
    }

    if(status_code)
    {
        printf("recv %d, but status code != 0 (%d)\n", type, status_code);\
        st++;
    }

    if(Checksum != p->CheckSum)
    {
        printf("recv %d, but checksum error, golden=%d, recved=%d\n", type, Checksum, p->CheckSum);
        st++;
    }

    return st;
}

int ServerMessenger::IsRecvL42(uint8_t *buf)
{
    FIX_L42_t *p = (FIX_L42_t *)buf;

    int st = 0;
    unsigned char Checksum;
    uint8_t type = FIXGET_UINT8(p->MsgHeader.MessageType);
    uint8_t status_code = FIXGET_UINT8(p->status_code);
    util.FIXSetCheckSum(&Checksum, (void*)buf, sizeof(FIX_L42_t));

    if(type != FIXMsgType_L42)
    {
        printf("Not recv L42, recved %d\n", type);
        st++;
    }

    if(status_code)
    {
        printf("recv %d, but status code != 0 (%d)\n", type, status_code);\
        st++;
    }

    if(Checksum != p->CheckSum)
    {
        printf("recv %d, but checksum error, golden=%d, recved=%d\n", type, Checksum, p->CheckSum);
        st++;
    }

    return st;
}

int ServerMessenger::IsRecvL60(uint8_t *buf)
{
    FIX_L60_t *p = (FIX_L60_t *)buf;

    int st = 0;
    unsigned char Checksum;
    uint8_t type = FIXGET_UINT8(p->MsgHeader.MessageType);
    uint8_t status_code = FIXGET_UINT8(p->status_code);
    util.FIXSetCheckSum(&Checksum, (void*)buf, sizeof(FIX_L60_t));

    if(type != FIXMsgType_L60)
    {
        printf("Not recv L60, recved %d\n", type);
        st++;
    }

    if(status_code)
    {
        printf("recv %d, but status code != 0 (%d)\n", type, status_code);\
        st++;
    }

    if(Checksum != p->CheckSum)
    {
        printf("recv %d, but checksum error, golden=%d, recved=%d\n", type, Checksum, p->CheckSum);
        st++;
    }

    return st;
}

int ServerMessenger::IsRecvR11(uint8_t *buf)
{
    FIX_R11_t *p = (FIX_R11_t *)buf;

    int st = 0;
    unsigned char Checksum;
    uint8_t type = FIXGET_UINT8(p->MsgHeader.MessageType);
    uint8_t status_code = FIXGET_UINT8(p->status_code);
    util.FIXSetCheckSum(&Checksum, (void*)buf, sizeof(FIX_R11_t));

    if(type != FIXMsgType_R11)
    {
        printf("Not recv R11, recved %d\n", type);
        st++;
    }

    if(status_code)
    {
        printf("recv %d, but status code != 0 (%d)\n", type, status_code);\
        st++;
    }

    if(Checksum != p->CheckSum)
    {
        printf("recv %d, but checksum error, golden=%d, recved=%d\n", type, Checksum, p->CheckSum);
        st++;
    }

    return st;
}

int ServerMessenger::IsRecvR05(uint8_t *buf)
{
    FIX_R05_t *p = (FIX_R05_t *)buf;

    int st = 0;
    unsigned char Checksum;
    uint8_t type = FIXGET_UINT8(p->MsgHeader.MessageType);
    uint8_t status_code = FIXGET_UINT8(p->status_code);
    util.FIXSetCheckSum(&Checksum, (void*)buf, sizeof(FIX_R05_t));

    if(type != FIXMsgType_R05)
    {
        printf("Not recv R05, recved %d\n", type);
        st++;
    }

    if(status_code)
    {
        printf("recv %d, but status code != 0 (%d)\n", type, status_code);\
        st++;
    }

    if(Checksum != p->CheckSum)
    {
        printf("recv %d, but checksum error, golden=%d, recved=%d\n", type, Checksum, p->CheckSum);
        st++;
    }

    return st;
}

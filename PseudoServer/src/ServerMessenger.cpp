#include "ServerMessenger.h"

int ServerMessenger::MakeL10(uint8_t *buf, uint16_t fcm_id, uint16_t session_id, uint8_t status_code)
{
    TMP_L10_t *p = (TMP_L10_t *)buf;
    util.TMPHdrSet(&p->MsgHeader, TMPMsgType_L10, sizeof(TMP_L10_t));
    TMPSET_UINT16(p->MsgHeader.fcm_id, fcm_id);
    TMPSET_UINT16(p->MsgHeader.session_id, session_id);
    p->status_code = status_code;
    p->start_in_bound_num = 0;
    util.TMPSetCheckSum(&p->CheckSum, (const void *)p, sizeof(TMP_L10_t));

    return sizeof(TMP_L10_t);
}

int ServerMessenger::MakeL30(uint8_t *buf, uint16_t fcm_id, uint16_t session_id, uint8_t status_code, uint32_t endOutBoundNum) 
{
    TMP_L30_t *p = (TMP_L30_t *)buf;
    util.TMPHdrSet(&p->MsgHeader, TMPMsgType_L30, sizeof(TMP_L30_t));
    TMPSET_UINT16(p->MsgHeader.fcm_id, fcm_id);
    TMPSET_UINT16(p->MsgHeader.session_id, session_id);
    p->status_code = status_code;
    p->end_in_bound_num = endOutBoundNum;
    p->EncryptMethod = 0;
    util.TMPSetCheckSum(&p->CheckSum, (const void *)p, sizeof(TMP_L30_t));

    return sizeof(TMP_L30_t);
}

int ServerMessenger::MakeL41(uint8_t *buf, uint16_t fcm_id, uint16_t session_id, uint8_t status_code, uint32_t fileSize, const std::vector<char>& data, bool isEof) 
{
    TMP_L41_t *p = (TMP_L41_t *)buf;
    util.TMPHdrSet(&p->MsgHeader, TMPMsgType_L41, sizeof(TMP_L41_t) + data.size());
    TMPSET_UINT16(p->MsgHeader.fcm_id, fcm_id);
    TMPSET_UINT16(p->MsgHeader.session_id, session_id);
    p->status_code = status_code;
    p->is_eof = isEof ? 1 : 0;
    p->file_size = fileSize;
    util.TMPSetCheckSum(&p->CheckSum, (const void *)p, sizeof(TMP_L41_t));
    memcpy(p->data, data.data(), data.size());

    return sizeof(TMP_L41_t) + data.size();
}

int ServerMessenger::MakeL50(uint8_t *buf, uint16_t fcm_id, uint16_t session_id, uint8_t status_code, uint8_t HeartBtInt, uint16_t max_flow_ctrl_cnt) 
{
    TMP_L50_t *p = (TMP_L50_t *)buf;
    util.TMPHdrSet(&p->MsgHeader, TMPMsgType_L50, sizeof(TMP_L50_t));
    TMPSET_UINT16(p->MsgHeader.fcm_id, fcm_id);
    TMPSET_UINT16(p->MsgHeader.session_id, session_id);
    p->status_code = status_code;
    p->HeartBtInt = HeartBtInt;
    p->max_flow_ctrl_cnt = max_flow_ctrl_cnt;
    util.TMPSetCheckSum(&p->CheckSum, (const void *)p, sizeof(TMP_L50_t));
    
    return sizeof(TMP_L50_t);
}

int ServerMessenger::MakeR12(uint8_t *buf, uint16_t fcm_id, uint16_t session_id, uint8_t status_code) 
{
    TMP_R12_t *p = (TMP_R12_t *)buf;
    util.TMPHdrSet(&p->MsgHeader, TMPMsgType_R12, sizeof(TMP_R12_t));
    TMPSET_UINT16(p->MsgHeader.fcm_id, fcm_id);
    TMPSET_UINT16(p->MsgHeader.session_id, session_id);
    p->status_code = status_code;
    util.TMPSetCheckSum(&p->CheckSum, (const void *)p, sizeof(TMP_R12_t));

    return sizeof(TMP_R12_t);
}

int ServerMessenger::MakeR04(uint8_t *buf, uint16_t fcm_id, uint16_t session_id, uint8_t status_code) 
{
    TMP_R04_t *p = (TMP_R04_t *)buf;
    util.TMPHdrSet(&p->MsgHeader, TMPMsgType_R04, sizeof(TMP_R04_t));
    TMPSET_UINT16(p->MsgHeader.fcm_id, fcm_id);
    TMPSET_UINT16(p->MsgHeader.session_id, session_id);
    p->status_code = status_code;
    util.TMPSetCheckSum(&p->CheckSum, (const void *)p, sizeof(TMP_R04_t));

    return sizeof(TMP_R04_t);
}

int ServerMessenger::MakeR05(uint8_t *buf, uint16_t fcm_id, uint16_t session_id, uint8_t status_code) 
{
    TMP_R05_t *p = (TMP_R05_t *)buf;
    util.TMPHdrSet(&p->MsgHeader, TMPMsgType_R05, sizeof(TMP_R05_t));
    TMPSET_UINT16(p->MsgHeader.fcm_id, fcm_id);
    TMPSET_UINT16(p->MsgHeader.session_id, session_id);
    p->status_code = status_code;
    util.TMPSetCheckSum(&p->CheckSum, (const void *)p, sizeof(TMP_R05_t));

    return sizeof(TMP_R05_t);
}

int ServerMessenger::MakeL20(uint8_t *buf, uint16_t fcm_id, uint16_t session_id, uint8_t status_code) 
{
    TMP_L20_t *p = (TMP_L20_t *)buf;
    util.TMPHdrSet(&p->MsgHeader, TMPMsgType_L20, sizeof(TMP_L20_t));
    TMPSET_UINT16(p->MsgHeader.fcm_id, fcm_id);
    TMPSET_UINT16(p->MsgHeader.session_id, session_id);
    p->status_code = status_code;
    util.TMPSetCheckSum(&p->CheckSum, (const void *)p, sizeof(TMP_L20_t));

    return sizeof(TMP_L20_t);
}

int ServerMessenger::MakeL40(uint8_t *buf, uint16_t fcm_id, uint16_t session_id, uint8_t status_code, uint16_t append_no) 
{
    TMP_L40_t *p = (TMP_L40_t *)buf;
    util.TMPHdrSet(&p->MsgHeader, TMPMsgType_L40, sizeof(TMP_L40_t));
    TMPSET_UINT16(p->MsgHeader.fcm_id, fcm_id);
    TMPSET_UINT16(p->MsgHeader.session_id, session_id);
    p->status_code = status_code;
    TMPSET_UINT16(p->append_no, append_no);
    util.TMPSetCheckSum(&p->CheckSum, (const void *)p, sizeof(TMP_L40_t));

    return sizeof(TMP_L40_t);
}

int ServerMessenger::MakeL60(uint8_t *buf, uint16_t fcm_id, uint16_t session_id, uint8_t status_code) 
{
    TMP_L60_t *p = (TMP_L60_t *)buf;
    util.TMPHdrSet(&p->MsgHeader, TMPMsgType_L60, sizeof(TMP_L60_t));
    TMPSET_UINT16(p->MsgHeader.fcm_id, fcm_id);
    TMPSET_UINT16(p->MsgHeader.session_id, session_id);
    p->status_code = status_code;
    util.TMPSetCheckSum(&p->CheckSum, (const void *)p, sizeof(TMP_L60_t));

    return sizeof(TMP_L60_t);
}

int ServerMessenger::IsRecvL10(uint8_t *buf)
{
    TMP_L10_t *p = (TMP_L10_t *)buf;

    int st = 0;
    unsigned char Checksum;
    uint8_t type = TMPGET_UINT8(p->MsgHeader.MessageType);
    uint8_t status_code = TMPGET_UINT8(p->status_code);
    util.TMPSetCheckSum(&Checksum, (void*)buf, sizeof(TMP_L10_t));

    if(type != TMPMsgType_L10)
    {
        printf("Not recv L10, recved %d\n", type);
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

int ServerMessenger::IsRecvL20(uint8_t *buf)
{
    TMP_L20_t *p = (TMP_L20_t *)buf;

    int st = 0;
    unsigned char Checksum;
    uint8_t type = TMPGET_UINT8(p->MsgHeader.MessageType);
    uint8_t status_code = TMPGET_UINT8(p->status_code);
    util.TMPSetCheckSum(&Checksum, (void*)buf, sizeof(TMP_L20_t));

    if(type != TMPMsgType_L20)
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
    TMP_L40_t *p = (TMP_L40_t *)buf;

    int st = 0;
    unsigned char Checksum;
    uint8_t type = TMPGET_UINT8(p->MsgHeader.MessageType);
    uint8_t status_code = TMPGET_UINT8(p->status_code);
    util.TMPSetCheckSum(&Checksum, (void*)buf, sizeof(TMP_L40_t));

    if(type != TMPMsgType_L40)
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
    TMP_L42_t *p = (TMP_L42_t *)buf;

    int st = 0;
    unsigned char Checksum;
    uint8_t type = TMPGET_UINT8(p->MsgHeader.MessageType);
    uint8_t status_code = TMPGET_UINT8(p->status_code);
    util.TMPSetCheckSum(&Checksum, (void*)buf, sizeof(TMP_L42_t));

    if(type != TMPMsgType_L42)
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
    TMP_L60_t *p = (TMP_L60_t *)buf;

    int st = 0;
    unsigned char Checksum;
    uint8_t type = TMPGET_UINT8(p->MsgHeader.MessageType);
    uint8_t status_code = TMPGET_UINT8(p->status_code);
    util.TMPSetCheckSum(&Checksum, (void*)buf, sizeof(TMP_L60_t));

    if(type != TMPMsgType_L60)
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
    TMP_R11_t *p = (TMP_R11_t *)buf;

    int st = 0;
    unsigned char Checksum;
    uint8_t type = TMPGET_UINT8(p->MsgHeader.MessageType);
    uint8_t status_code = TMPGET_UINT8(p->status_code);
    util.TMPSetCheckSum(&Checksum, (void*)buf, sizeof(TMP_R11_t));

    if(type != TMPMsgType_R11)
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
    TMP_R05_t *p = (TMP_R05_t *)buf;

    int st = 0;
    unsigned char Checksum;
    uint8_t type = TMPGET_UINT8(p->MsgHeader.MessageType);
    uint8_t status_code = TMPGET_UINT8(p->status_code);
    util.TMPSetCheckSum(&Checksum, (void*)buf, sizeof(TMP_R05_t));

    if(type != TMPMsgType_R05)
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

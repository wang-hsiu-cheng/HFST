#include "TaifexOrderUtility.h"

void TaifexOrderUtility::TMPHdrSet(TMPhdr_t* hdr, uint16_t msgType, uint16_t msgLen)
{
    TMPSET_MSGLEN(hdr->msg_length, msgLen);
	TMPSET_UINT8(hdr->MessageType, msgType);
	TMPSET_INT32(hdr->msg_time.epoch_s, time(NULL));
}

void TaifexOrderUtility::TMPSetCheckSum(uint8_t *checksum, const void *data, size_t size)
{
    char *ptr = (char *)data;
	char *End = ptr + size - 1;
	unsigned int Sum = 0;

	for (; ptr != End; (Sum += (unsigned char)(*(ptr++))));

	(*checksum) = (uint8_t)(Sum & 255);

	return;
}

int TaifexOrderUtility::GetMsgLen(TMPhdr_t* hdr)
{
    uint8_t type = TMPGET_UINT8(hdr->MessageType);
    int len = 0;
    switch(type)
    {
        case TMPMsgType_R01: len = sizeof(TMP_R01_t); break;
        case TMPMsgType_R07: len = sizeof(TMP_R07_t); break;
        case TMPMsgType_R09: len = sizeof(TMP_R09_t); break;
        case TMPMsgType_R02: len = sizeof(TMP_R02_t); break;
        case TMPMsgType_R03: len = sizeof(TMP_R03_t); break;
        case TMPMsgType_R08: len = sizeof(TMP_R08_t); break;
        case TMPMsgType_R04: len = sizeof(TMP_R04_t); break;
        case TMPMsgType_R05: len = sizeof(TMP_R05_t); break;
        case TMPMsgType_R11: len = sizeof(TMP_R11_t); break;
        case TMPMsgType_R12: len = sizeof(TMP_R12_t); break;
        case TMPMsgType_L10: len = sizeof(TMP_L10_t); break;
        case TMPMsgType_L20: len = sizeof(TMP_L20_t); break;
        case TMPMsgType_L30: len = sizeof(TMP_L30_t); break;
        case TMPMsgType_L40: len = sizeof(TMP_L40_t); break;
        case TMPMsgType_L41: len = sizeof(TMP_L41_t); break;
        case TMPMsgType_L42: len = sizeof(TMP_L42_t); break;
        case TMPMsgType_L50: len = sizeof(TMP_L50_t); break;
        case TMPMsgType_L60: len = sizeof(TMP_L60_t); break;
    };

    return len;
}

char* TaifexOrderUtility::TMPTimeToString(time_t curSec)
{
    struct tm *curDate;
    curDate = localtime(&curSec);
    sprintf(dateString,
        "%02d:%02d:%02d, %s %02d/%02d-%d",
        curDate->tm_hour,
        curDate->tm_min,
        curDate->tm_sec,
        Weekdays[curDate->tm_wday],
        curDate->tm_mon+1,
        curDate->tm_mday,
        curDate->tm_year+1900);
    return dateString;
}

char* TaifexOrderUtility::GetTimeStr(time_t msg_time_sec, uint16_t msg_time_ms)
{
	time_t curtime;
	struct tm * timeinfo;
	
	curtime = time(&msg_time_sec);
	
	timeinfo = localtime (&curtime);
	strftime(timestr,sizeof(timestr),"%Y-%m-%d %H:%M:%S",timeinfo);
	sprintf(&timestr[strlen(timestr)], ".%03u",msg_time_ms);
	
	return timestr;
}


void TaifexOrderUtility::HexDump(FILE *fptr, const char *desc, void *addr, int len)
{
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char *)addr;
    
    // Output description if given.
    if (desc != NULL)
        fprintf(fptr, "%s:\n", desc);

    if (len == 0)
    {
        fprintf(fptr, "  ZERO LENGTH\n");
        return;
    }
    if (len < 0)
    {
        fprintf(fptr, "  NEGATIVE LENGTH: %i\n", len);
        return;
    }

    // Process every byte in the data.
    for (i = 0; i < len; i++)
    {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0)
        {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                fprintf(fptr, "  %s\n", buff);

            // Output the offset.
            fprintf(fptr, "  %04x ", i);
        }

        // Now the hex code for the specific character.
        fprintf(fptr, " %02x", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0)
    {
        fprintf(fptr, "   ");
        i++;
    }

    // And print the final ASCII bit.
    fprintf(fptr, "  %s\n", buff);
}

void TaifexOrderUtility::PrintTMPPkt(FILE* fptr, const void* data)
{
	time_t msg_time_sec;
	int i;
	uint8_t type;
	TMPhdr_t *phdr = (TMPhdr_t*)data;
	
	type = phdr->MessageType;

	if(type == TMPMsgType_R02)
	{
		TMP_R02_t* TMPR02_ptr = (TMP_R02_t*)data;
		time_t msg_time_sec;
		time_t org_trans_msg_time_sec;
		
		// fprintf(fptr, "Taifex R02:\n");
		fprintf(fptr, "msg_length=%d|", TMPGET_UINT16(TMPR02_ptr->MsgHeader.msg_length));
		fprintf(fptr, "MsgSeqNum=%d|", TMPGET_UINT32(TMPR02_ptr->MsgHeader.MsgSeqNum));
		fprintf(fptr, "MessageType=%d|", TMPGET_UINT8(TMPR02_ptr->MsgHeader.MessageType));
		msg_time_sec = TMPGET_UINT32(TMPR02_ptr->MsgHeader.msg_time.epoch_s);
		fprintf(fptr, "msg_time=%s|", GetTimeStr(msg_time_sec, TMPGET_UINT16(TMPR02_ptr->MsgHeader.msg_time.ms)));
		//fprintf(fptr, "fcm_id=%d|", TMPGET_UINT16(TMPR02_ptr->MsgHeader.fcm_id));
		fprintf(fptr, "session_id=%d|", TMPGET_UINT16(TMPR02_ptr->MsgHeader.session_id));
		
		fprintf(fptr, "status_code=%d|",TMPR02_ptr->status_code);
		fprintf(fptr, "ExecType=%c|",TMPR02_ptr->ExecType);
		fprintf(fptr, "cm_id=%d|",TMPGET_UINT16(TMPR02_ptr->cm_id));
		fprintf(fptr, "fcm_id=%d|",TMPGET_UINT16(TMPR02_ptr->fcm_id));
		fprintf(fptr, "order_no=");
		for(i = 0; i < TMP_ORDNO_LEN; i++)
			fprintf(fptr, "%c",TMPR02_ptr->order_no[i]);
		fprintf(fptr, "|");	
		fprintf(fptr, "ord_id=%d|",TMPGET_UINT32(TMPR02_ptr->ord_id));
		fprintf(fptr, "user_define=");
		for(i = 0; i < TMP_UDD_LEN; i++)	
			fprintf(fptr, "%c",TMPR02_ptr->user_define[i]);
		fprintf(fptr, "|");
		fprintf(fptr, "symbol_type=%d|",TMPR02_ptr->symbol_type);	
		fprintf(fptr, "sym=");
		for(i = 0; i < TMP_SYM_LEN; i++)
			fprintf(fptr, "%c",TMPR02_ptr->sym[i]);
		fprintf(fptr, "|");
		fprintf(fptr, "Price=%d|",TMPGET_INT32(TMPR02_ptr->Price));
		fprintf(fptr, "qty=%u|",TMPGET_UINT16(TMPR02_ptr->qty));
		fprintf(fptr, "investor_acno=%d|",TMPGET_UINT32(TMPR02_ptr->investor_acno));
		fprintf(fptr, "investor_flag=%c|",TMPR02_ptr->investor_flag);
		fprintf(fptr, "Side=%d|",TMPR02_ptr->Side);
		fprintf(fptr, "OrdType=%d|",TMPR02_ptr->OrdType);
		fprintf(fptr, "TimeInForce=%u|",TMPR02_ptr->TimeInForce);
		fprintf(fptr, "PositionEffect=%c|",TMPR02_ptr->PositionEffect);	
		
		fprintf(fptr, "LastPx=%d|", TMPGET_INT32(TMPR02_ptr->LastPx));
		fprintf(fptr, "LastQty=%u|", TMPGET_UINT16(TMPR02_ptr->LastQty));
		fprintf(fptr, "px_subtotal=%ld|", TMPGET_INT64(TMPR02_ptr->px_subtotal));
		fprintf(fptr, "CumQty=%u|", TMPGET_UINT16(TMPR02_ptr->CumQty));
		fprintf(fptr, "LeavesQty=%u|", TMPGET_UINT16(TMPR02_ptr->LeavesQty));
		fprintf(fptr, "before_qty=%u|", TMPGET_UINT16(TMPR02_ptr->before_qty));
		fprintf(fptr, "leg_side_1=%u|", TMPGET_UINT8(TMPR02_ptr->leg_side_1));
		fprintf(fptr, "leg_side_2=%u|", TMPGET_UINT8(TMPR02_ptr->leg_side_2));
		fprintf(fptr, "leg_px_1=%d|", TMPGET_INT32(TMPR02_ptr->leg_px_1));
		fprintf(fptr, "leg_px_2=%d|", TMPGET_INT32(TMPR02_ptr->leg_px_2));
		fprintf(fptr, "leg_qty_1=%u|", TMPGET_UINT16(TMPR02_ptr->leg_qty_1));
		fprintf(fptr, "leg_qty_2=%u|", TMPGET_UINT16(TMPR02_ptr->leg_qty_2));
		
		org_trans_msg_time_sec = TMPGET_UINT32(TMPR02_ptr->org_trans_time.epoch_s);
		fprintf(fptr, "org_trans_time=%s|", GetTimeStr(org_trans_msg_time_sec, TMPGET_UINT16(TMPR02_ptr->org_trans_time.ms)));
		msg_time_sec = TMPGET_UINT32(TMPR02_ptr->TransactTime.epoch_s);
		fprintf(fptr, "TransactTime=%s|", GetTimeStr(msg_time_sec, TMPGET_UINT16(TMPR02_ptr->TransactTime.ms)));
		fprintf(fptr, "target_id=%u|", TMPR02_ptr->target_id);
		fprintf(fptr, "uniq_id=%u|", TMPGET_UINT32(TMPR02_ptr->uniq_id));
		fprintf(fptr, "rpt_seq=%u|", TMPGET_UINT32(TMPR02_ptr->rpt_seq));
		fprintf(fptr, "protocol_type=%d|", TMPR02_ptr->protocol_type);
		fprintf(fptr, "CheckSum=%03u",TMPR02_ptr->CheckSum);
		fprintf(fptr, "\n");
	}
	else if(type == TMPMsgType_R03)
	{
		TMP_R03_t* TMPR03_ptr = (TMP_R03_t*)data;
		
		// fprintf(fptr, "Taifex R03:\n");
				fprintf(fptr, "msg_length=%d|", TMPGET_UINT16(TMPR03_ptr->MsgHeader.msg_length));
		fprintf(fptr, "MsgSeqNum=%d|", TMPGET_UINT32(TMPR03_ptr->MsgHeader.MsgSeqNum));
		fprintf(fptr, "MessageType=%d|", TMPGET_UINT8(TMPR03_ptr->MsgHeader.MessageType));
		msg_time_sec = TMPGET_UINT32(TMPR03_ptr->MsgHeader.msg_time.epoch_s);
		fprintf(fptr, "msg_time=%s|", GetTimeStr(msg_time_sec, TMPGET_UINT16(TMPR03_ptr->MsgHeader.msg_time.ms)));
		//fprintf(fptr, "fcm_id=%d|", TMPGET_UINT16(TMPR03_ptr->MsgHeader.fcm_id));
		fprintf(fptr, "session_id=%d|", TMPGET_UINT16(TMPR03_ptr->MsgHeader.session_id));
		
		fprintf(fptr, "status_code=%d|",TMPR03_ptr->status_code);
		fprintf(fptr, "ExecType=%c|",TMPR03_ptr->ExecType);
		fprintf(fptr, "fcm_id=%d|",TMPGET_UINT16(TMPR03_ptr->fcm_id));
		fprintf(fptr, "order_no=");
		for(i = 0; i < TMP_ORDNO_LEN; i++)
			fprintf(fptr, "%c",TMPR03_ptr->order_no[i]);
		fprintf(fptr, "|");	
		fprintf(fptr, "ord_id=%d|",TMPGET_UINT32(TMPR03_ptr->ord_id));
		fprintf(fptr, "user_define=");
		for(i = 0; i < TMP_UDD_LEN; i++)	
			fprintf(fptr, "%c",TMPR03_ptr->user_define[i]);
		fprintf(fptr, "|");
		fprintf(fptr, "rpt_seq=%u|", TMPGET_UINT32(TMPR03_ptr->rpt_seq));
		fprintf(fptr, "Side=%d|",TMPR03_ptr->Side);
		fprintf(fptr, "CheckSum=%03u",TMPR03_ptr->CheckSum);
		fprintf(fptr, "\n");
	}
    else if(type == TMPMsgType_R01)
	{
		TMP_R01_t* TMPR01_ptr = (TMP_R01_t*)data;
		time_t msg_time_sec;
		
		// fprintf(fptr, "Taifex R01:\n");
		fprintf(fptr, "msg_length=%d|", TMPGET_UINT16(TMPR01_ptr->MsgHeader.msg_length));
		fprintf(fptr, "MsgSeqNum=%d|", TMPGET_UINT32(TMPR01_ptr->MsgHeader.MsgSeqNum));
		fprintf(fptr, "MessageType=%d|", TMPGET_UINT8(TMPR01_ptr->MsgHeader.MessageType));
		msg_time_sec = TMPGET_UINT32(TMPR01_ptr->MsgHeader.msg_time.epoch_s);
		fprintf(fptr, "msg_time=%s|", GetTimeStr(msg_time_sec, TMPGET_UINT16(TMPR01_ptr->MsgHeader.msg_time.ms)));
		//fprintf(fptr, "fcm_id=%d|", TMPGET_UINT16(TMPR01_ptr->MsgHeader.fcm_id));
		fprintf(fptr, "session_id=%d|", TMPGET_UINT16(TMPR01_ptr->MsgHeader.session_id));
		
		fprintf(fptr, "ExecType=%c|",TMPR01_ptr->ExecType);
		fprintf(fptr, "cm_id=%d|",TMPGET_UINT16(TMPR01_ptr->cm_id));
		fprintf(fptr, "fcm_id=%d|",TMPGET_UINT16(TMPR01_ptr->fcm_id));
		fprintf(fptr, "order_no=");
		for(i = 0; i < TMP_ORDNO_LEN; i++)
			fprintf(fptr, "%c",TMPR01_ptr->order_no[i]);
		fprintf(fptr, "|");	
		fprintf(fptr, "ord_id=%d|",TMPGET_UINT32(TMPR01_ptr->ord_id));
		fprintf(fptr, "user_define=");
		for(i = 0; i < TMP_UDD_LEN; i++)	
			fprintf(fptr, "%c",TMPR01_ptr->user_define[i]);
		fprintf(fptr, "|");
		fprintf(fptr, "symbol_type=%d|",TMPR01_ptr->symbol_type);	
		fprintf(fptr, "sym=");
		for(i = 0; i < TMP_SYM_LEN; i++)
			fprintf(fptr, "%c",TMPR01_ptr->sym[i]);
		fprintf(fptr, "|");
		fprintf(fptr, "Price=%d|",TMPGET_INT32(TMPR01_ptr->Price));
		fprintf(fptr, "qty=%u|",TMPGET_UINT16(TMPR01_ptr->qty));
		fprintf(fptr, "investor_acno=%d|",TMPGET_UINT32(TMPR01_ptr->investor_acno));
		fprintf(fptr, "investor_flag=%c|",TMPR01_ptr->investor_flag);
		fprintf(fptr, "Side=%d|",TMPR01_ptr->Side);
		fprintf(fptr, "OrdType=%d|",TMPR01_ptr->OrdType);
		fprintf(fptr, "TimeInForce=%u|",TMPR01_ptr->TimeInForce);
		fprintf(fptr, "PositionEffect=%c|",TMPR01_ptr->PositionEffect);	
        fprintf(fptr, "order_source=%c|",TMPR01_ptr->order_source);
        fprintf(fptr, "info_source=%c%c%c|",TMPR01_ptr->info_source[0], TMPR01_ptr->info_source[1], TMPR01_ptr->info_source[2]);	
        fprintf(fptr, "CheckSum=%03u",TMPR01_ptr->CheckSum);
		fprintf(fptr, "\n");
	}
    else if(type == TMPMsgType_R09)
	{
		TMP_R09_t* TMPR09_ptr = (TMP_R09_t*)data;
		time_t msg_time_sec;
		
		// fprintf(fptr, "Taifex R09:\n");
		fprintf(fptr, "msg_length=%d|", TMPGET_UINT16(TMPR09_ptr->MsgHeader.msg_length));
		fprintf(fptr, "MsgSeqNum=%d|", TMPGET_UINT32(TMPR09_ptr->MsgHeader.MsgSeqNum));
		fprintf(fptr, "MessageType=%d|", TMPGET_UINT8(TMPR09_ptr->MsgHeader.MessageType));
		msg_time_sec = TMPGET_UINT32(TMPR09_ptr->MsgHeader.msg_time.epoch_s);
		fprintf(fptr, "msg_time=%s|", GetTimeStr(msg_time_sec, TMPGET_UINT16(TMPR09_ptr->MsgHeader.msg_time.ms)));
		//fprintf(fptr, "fcm_id=%d|", TMPGET_UINT16(TMPR09_ptr->MsgHeader.fcm_id));
		fprintf(fptr, "session_id=%d|", TMPGET_UINT16(TMPR09_ptr->MsgHeader.session_id));
		
		fprintf(fptr, "ExecType=%c|",TMPR09_ptr->ExecType);
		fprintf(fptr, "cm_id=%d|",TMPGET_UINT16(TMPR09_ptr->cm_id));
		fprintf(fptr, "fcm_id=%d|",TMPGET_UINT16(TMPR09_ptr->fcm_id));
		fprintf(fptr, "order_no=");
		for(i = 0; i < TMP_ORDNO_LEN; i++)
			fprintf(fptr, "%c",TMPR09_ptr->order_no[i]);
		fprintf(fptr, "|");	
		fprintf(fptr, "ord_id=%d|",TMPGET_UINT32(TMPR09_ptr->ord_id));
		fprintf(fptr, "user_define=");
		for(i = 0; i < TMP_UDD_LEN; i++)	
			fprintf(fptr, "%c",TMPR09_ptr->user_define[i]);
		fprintf(fptr, "|");
		fprintf(fptr, "symbol_type=%d|",TMPR09_ptr->symbol_type);	
		fprintf(fptr, "sym=");
		for(i = 0; i < TMP_SYM_LEN; i++)
			fprintf(fptr, "%c",TMPR09_ptr->sym[i]);
		fprintf(fptr, "|");
		fprintf(fptr, "BidPx=%d|",TMPGET_INT32(TMPR09_ptr->BidPx));
        fprintf(fptr, "OfferPx=%d|",TMPGET_INT32(TMPR09_ptr->OfferPx));
        fprintf(fptr, "BidSize=%d|",TMPGET_UINT16(TMPR09_ptr->BidSize));
        fprintf(fptr, "OfferSize=%d|",TMPGET_UINT16(TMPR09_ptr->OfferSize));
		fprintf(fptr, "investor_acno=%d|",TMPGET_UINT32(TMPR09_ptr->investor_acno));
		fprintf(fptr, "investor_flag=%c|",TMPR09_ptr->investor_flag);
		fprintf(fptr, "TimeInForce=%u|",TMPR09_ptr->TimeInForce);
		fprintf(fptr, "PositionEffect=%c|",TMPR09_ptr->PositionEffect);	
        fprintf(fptr, "order_source=%c|",TMPR09_ptr->order_source);
        fprintf(fptr, "info_source=%c%c%c|",TMPR09_ptr->info_source[0], TMPR09_ptr->info_source[1], TMPR09_ptr->info_source[2]);	
        fprintf(fptr, "CheckSum=%03u",TMPR09_ptr->CheckSum);
		fprintf(fptr, "\n");
	}
    else if(type == TMPMsgType_R04 || type == TMPMsgType_R05)
    {
        TMP_R05_t* TMPR05_ptr = (TMP_R05_t*)data;
		time_t msg_time_sec;
        // fprintf(fptr, "Taifex HB:\n");
        fprintf(fptr, "msg_length=%d|", TMPGET_UINT16(TMPR05_ptr->MsgHeader.msg_length));
		fprintf(fptr, "MsgSeqNum=%d|", TMPGET_UINT32(TMPR05_ptr->MsgHeader.MsgSeqNum));
		fprintf(fptr, "MessageType=%d|", TMPGET_UINT8(TMPR05_ptr->MsgHeader.MessageType));
		msg_time_sec = TMPGET_UINT32(TMPR05_ptr->MsgHeader.msg_time.epoch_s);
		fprintf(fptr, "msg_time=%s|", GetTimeStr(msg_time_sec, TMPGET_UINT16(TMPR05_ptr->MsgHeader.msg_time.ms)));
		//fprintf(fptr, "fcm_id=%d|", TMPGET_UINT16(TMPR05_ptr->MsgHeader.fcm_id));
		fprintf(fptr, "session_id=%d|", TMPGET_UINT16(TMPR05_ptr->MsgHeader.session_id));
        fprintf(fptr, "status_code=%d|", TMPGET_UINT16(TMPR05_ptr->status_code));
        fprintf(fptr, "CheckSum=%03u",TMPR05_ptr->CheckSum);
        fprintf(fptr, "\n");
    }
	else
	{
		fprintf(fptr, "xOrder Taifex Unknown Type Packet:\n");
        uint8_t *ptr = (uint8_t*)data;
		for(i = 0; i < 64 * 3; i++)
		{
			fprintf(fptr, "%02x", ptr[i]);
			if(i % 64 == 63)
				fprintf(fptr, "\n");
		}
		fprintf(fptr, "\n");
	}
}
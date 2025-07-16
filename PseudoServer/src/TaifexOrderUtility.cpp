#include "TaifexOrderUtility.h"

void TaifexOrderUtility::FIXHdrSet(FIXhdr_t* hdr, uint16_t msgType, uint16_t msgLen)
{
    FIXSET_MSGLEN(hdr->msg_length, msgLen);
	FIXSET_UINT8(hdr->MessageType, msgType);
	FIXSET_INT32(hdr->SendingTime.epoch_s, time(NULL));
}

void TaifexOrderUtility::FIXSetCheckSum(uint8_t *checksum, const void *data, size_t size)
{
    char *ptr = (char *)data;
	char *End = ptr + size - 1;
	unsigned int Sum = 0;

	for (; ptr != End; (Sum += (unsigned char)(*(ptr++))));

	(*checksum) = (uint8_t)(Sum & 255);

	return;
}

int TaifexOrderUtility::GetMsgLen(FIXhdr_t* hdr)
{
    uint8_t type = FIXGET_UINT8(hdr->MessageType);
    int len = 0;
    switch(type)
    {
        case FIXMsgType_R01: len = sizeof(FIX_R01_t); break;
        case FIXMsgType_R07: len = sizeof(FIX_R07_t); break;
        case FIXMsgType_R09: len = sizeof(FIX_R09_t); break;
        case FIXMsgType_R02: len = sizeof(FIX_R02_t); break;
        case FIXMsgType_R03: len = sizeof(FIX_R03_t); break;
        case FIXMsgType_R08: len = sizeof(FIX_R08_t); break;
        case FIXMsgType_R04: len = sizeof(FIX_R04_t); break;
        case FIXMsgType_R05: len = sizeof(FIX_R05_t); break;
        case FIXMsgType_R11: len = sizeof(FIX_R11_t); break;
        case FIXMsgType_R12: len = sizeof(FIX_R12_t); break;
        case FIXMsgType_L10: len = sizeof(FIX_L10_t); break;
        case FIXMsgType_L20: len = sizeof(FIX_L20_t); break;
        case FIXMsgType_L30: len = sizeof(FIX_L30_t); break;
        case FIXMsgType_L40: len = sizeof(FIX_L40_t); break;
        case FIXMsgType_L41: len = sizeof(FIX_L41_t); break;
        case FIXMsgType_L42: len = sizeof(FIX_L42_t); break;
        case FIXMsgType_L50: len = sizeof(FIX_L50_t); break;
        case FIXMsgType_L60: len = sizeof(FIX_L60_t); break;
    };

    return len;
}

char* TaifexOrderUtility::FIXTimeToString(time_t curSec)
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

void TaifexOrderUtility::PrintFIXPkt(FILE* fptr, const void* data)
{
	time_t msg_time_sec;
	int i;
	uint8_t type;
	FIXhdr_t *phdr = (FIXhdr_t*)data;
	
	type = phdr->MessageType;

	if(type == FIXMsgType_R02)
	{
		FIX_R02_t* FIXR02_ptr = (FIX_R02_t*)data;
		time_t msg_time_sec;
		time_t org_trans_msg_time_sec;
		
		// fprintf(fptr, "Taifex R02:\n");
		fprintf(fptr, "msg_length=%d|", FIXGET_UINT16(FIXR02_ptr->MsgHeader.msg_length));
		fprintf(fptr, "MsgSeqNum=%d|", FIXGET_UINT32(FIXR02_ptr->MsgHeader.MsgSeqNum));
		fprintf(fptr, "MessageType=%d|", FIXGET_UINT8(FIXR02_ptr->MsgHeader.MessageType));
		msg_time_sec = FIXGET_UINT32(FIXR02_ptr->MsgHeader.SendingTime.epoch_s);
		fprintf(fptr, "SendingTime=%s|", GetTimeStr(msg_time_sec, FIXGET_UINT16(FIXR02_ptr->MsgHeader.SendingTime.ms)));
		//fprintf(fptr, "TargetCompID=%d|", FIXGET_UINT16(FIXR02_ptr->MsgHeader.TargetCompID));
		fprintf(fptr, "session_id=%d|", FIXGET_UINT16(FIXR02_ptr->MsgHeader.session_id));
		
		fprintf(fptr, "status_code=%d|",FIXR02_ptr->status_code);
		fprintf(fptr, "ExecType=%c|",FIXR02_ptr->ExecType);
		fprintf(fptr, "cm_id=%d|",FIXGET_UINT16(FIXR02_ptr->cm_id));
		fprintf(fptr, "TargetCompID=%d|",FIXGET_UINT16(FIXR02_ptr->TargetCompID));
		fprintf(fptr, "order_no=");
		for(i = 0; i < FIX_ORDNO_LEN; i++)
			fprintf(fptr, "%c",FIXR02_ptr->order_no[i]);
		fprintf(fptr, "|");	
		fprintf(fptr, "ord_id=%d|",FIXGET_UINT32(FIXR02_ptr->ord_id));
		fprintf(fptr, "user_define=");
		for(i = 0; i < FIX_UDD_LEN; i++)	
			fprintf(fptr, "%c",FIXR02_ptr->user_define[i]);
		fprintf(fptr, "|");
		fprintf(fptr, "symbol_type=%d|",FIXR02_ptr->symbol_type);	
		fprintf(fptr, "sym=");
		for(i = 0; i < FIX_SYM_LEN; i++)
			fprintf(fptr, "%c",FIXR02_ptr->sym[i]);
		fprintf(fptr, "|");
		fprintf(fptr, "Price=%d|",FIXGET_INT32(FIXR02_ptr->Price));
		fprintf(fptr, "qty=%u|",FIXGET_UINT16(FIXR02_ptr->qty));
		fprintf(fptr, "investor_acno=%d|",FIXGET_UINT32(FIXR02_ptr->investor_acno));
		fprintf(fptr, "investor_flag=%c|",FIXR02_ptr->investor_flag);
		fprintf(fptr, "Side=%d|",FIXR02_ptr->Side);
		fprintf(fptr, "OrdType=%d|",FIXR02_ptr->OrdType);
		fprintf(fptr, "TimeInForce=%u|",FIXR02_ptr->TimeInForce);
		fprintf(fptr, "PositionEffect=%c|",FIXR02_ptr->PositionEffect);	
		
		fprintf(fptr, "LastPx=%d|", FIXGET_INT32(FIXR02_ptr->LastPx));
		fprintf(fptr, "LastQty=%u|", FIXGET_UINT16(FIXR02_ptr->LastQty));
		fprintf(fptr, "px_subtotal=%ld|", FIXGET_INT64(FIXR02_ptr->px_subtotal));
		fprintf(fptr, "CumQty=%u|", FIXGET_UINT16(FIXR02_ptr->CumQty));
		fprintf(fptr, "LeavesQty=%u|", FIXGET_UINT16(FIXR02_ptr->LeavesQty));
		fprintf(fptr, "before_qty=%u|", FIXGET_UINT16(FIXR02_ptr->before_qty));
		fprintf(fptr, "leg_side_1=%u|", FIXGET_UINT8(FIXR02_ptr->leg_side_1));
		fprintf(fptr, "leg_side_2=%u|", FIXGET_UINT8(FIXR02_ptr->leg_side_2));
		fprintf(fptr, "leg_px_1=%d|", FIXGET_INT32(FIXR02_ptr->leg_px_1));
		fprintf(fptr, "leg_px_2=%d|", FIXGET_INT32(FIXR02_ptr->leg_px_2));
		fprintf(fptr, "leg_qty_1=%u|", FIXGET_UINT16(FIXR02_ptr->leg_qty_1));
		fprintf(fptr, "leg_qty_2=%u|", FIXGET_UINT16(FIXR02_ptr->leg_qty_2));
		
		org_trans_msg_time_sec = FIXGET_UINT32(FIXR02_ptr->org_trans_time.epoch_s);
		fprintf(fptr, "org_trans_time=%s|", GetTimeStr(org_trans_msg_time_sec, FIXGET_UINT16(FIXR02_ptr->org_trans_time.ms)));
		msg_time_sec = FIXGET_UINT32(FIXR02_ptr->TransactTime.epoch_s);
		fprintf(fptr, "TransactTime=%s|", GetTimeStr(msg_time_sec, FIXGET_UINT16(FIXR02_ptr->TransactTime.ms)));
		fprintf(fptr, "target_id=%u|", FIXR02_ptr->target_id);
		fprintf(fptr, "uniq_id=%u|", FIXGET_UINT32(FIXR02_ptr->uniq_id));
		fprintf(fptr, "rpt_seq=%u|", FIXGET_UINT32(FIXR02_ptr->rpt_seq));
		fprintf(fptr, "protocol_type=%d|", FIXR02_ptr->protocol_type);
		fprintf(fptr, "CheckSum=%03u",FIXR02_ptr->CheckSum);
		fprintf(fptr, "\n");
	}
	else if(type == FIXMsgType_R03)
	{
		FIX_R03_t* FIXR03_ptr = (FIX_R03_t*)data;
		
		// fprintf(fptr, "Taifex R03:\n");
				fprintf(fptr, "msg_length=%d|", FIXGET_UINT16(FIXR03_ptr->MsgHeader.msg_length));
		fprintf(fptr, "MsgSeqNum=%d|", FIXGET_UINT32(FIXR03_ptr->MsgHeader.MsgSeqNum));
		fprintf(fptr, "MessageType=%d|", FIXGET_UINT8(FIXR03_ptr->MsgHeader.MessageType));
		msg_time_sec = FIXGET_UINT32(FIXR03_ptr->MsgHeader.SendingTime.epoch_s);
		fprintf(fptr, "SendingTime=%s|", GetTimeStr(msg_time_sec, FIXGET_UINT16(FIXR03_ptr->MsgHeader.SendingTime.ms)));
		//fprintf(fptr, "TargetCompID=%d|", FIXGET_UINT16(FIXR03_ptr->MsgHeader.TargetCompID));
		fprintf(fptr, "session_id=%d|", FIXGET_UINT16(FIXR03_ptr->MsgHeader.session_id));
		
		fprintf(fptr, "status_code=%d|",FIXR03_ptr->status_code);
		fprintf(fptr, "ExecType=%c|",FIXR03_ptr->ExecType);
		fprintf(fptr, "TargetCompID=%d|",FIXGET_UINT16(FIXR03_ptr->TargetCompID));
		fprintf(fptr, "order_no=");
		for(i = 0; i < FIX_ORDNO_LEN; i++)
			fprintf(fptr, "%c",FIXR03_ptr->order_no[i]);
		fprintf(fptr, "|");	
		fprintf(fptr, "ord_id=%d|",FIXGET_UINT32(FIXR03_ptr->ord_id));
		fprintf(fptr, "user_define=");
		for(i = 0; i < FIX_UDD_LEN; i++)	
			fprintf(fptr, "%c",FIXR03_ptr->user_define[i]);
		fprintf(fptr, "|");
		fprintf(fptr, "rpt_seq=%u|", FIXGET_UINT32(FIXR03_ptr->rpt_seq));
		fprintf(fptr, "Side=%d|",FIXR03_ptr->Side);
		fprintf(fptr, "CheckSum=%03u",FIXR03_ptr->CheckSum);
		fprintf(fptr, "\n");
	}
    else if(type == FIXMsgType_R01)
	{
		FIX_R01_t* FIXR01_ptr = (FIX_R01_t*)data;
		time_t msg_time_sec;
		
		// fprintf(fptr, "Taifex R01:\n");
		fprintf(fptr, "msg_length=%d|", FIXGET_UINT16(FIXR01_ptr->MsgHeader.msg_length));
		fprintf(fptr, "MsgSeqNum=%d|", FIXGET_UINT32(FIXR01_ptr->MsgHeader.MsgSeqNum));
		fprintf(fptr, "MessageType=%d|", FIXGET_UINT8(FIXR01_ptr->MsgHeader.MessageType));
		msg_time_sec = FIXGET_UINT32(FIXR01_ptr->MsgHeader.SendingTime.epoch_s);
		fprintf(fptr, "SendingTime=%s|", GetTimeStr(msg_time_sec, FIXGET_UINT16(FIXR01_ptr->MsgHeader.SendingTime.ms)));
		//fprintf(fptr, "TargetCompID=%d|", FIXGET_UINT16(FIXR01_ptr->MsgHeader.TargetCompID));
		fprintf(fptr, "session_id=%d|", FIXGET_UINT16(FIXR01_ptr->MsgHeader.session_id));
		
		fprintf(fptr, "ExecType=%c|",FIXR01_ptr->ExecType);
		fprintf(fptr, "cm_id=%d|",FIXGET_UINT16(FIXR01_ptr->cm_id));
		fprintf(fptr, "TargetCompID=%d|",FIXGET_UINT16(FIXR01_ptr->TargetCompID));
		fprintf(fptr, "order_no=");
		for(i = 0; i < FIX_ORDNO_LEN; i++)
			fprintf(fptr, "%c",FIXR01_ptr->order_no[i]);
		fprintf(fptr, "|");	
		fprintf(fptr, "ord_id=%d|",FIXGET_UINT32(FIXR01_ptr->ord_id));
		fprintf(fptr, "user_define=");
		for(i = 0; i < FIX_UDD_LEN; i++)	
			fprintf(fptr, "%c",FIXR01_ptr->user_define[i]);
		fprintf(fptr, "|");
		fprintf(fptr, "symbol_type=%d|",FIXR01_ptr->symbol_type);	
		fprintf(fptr, "sym=");
		for(i = 0; i < FIX_SYM_LEN; i++)
			fprintf(fptr, "%c",FIXR01_ptr->sym[i]);
		fprintf(fptr, "|");
		fprintf(fptr, "Price=%d|",FIXGET_INT32(FIXR01_ptr->Price));
		fprintf(fptr, "qty=%u|",FIXGET_UINT16(FIXR01_ptr->qty));
		fprintf(fptr, "investor_acno=%d|",FIXGET_UINT32(FIXR01_ptr->investor_acno));
		fprintf(fptr, "investor_flag=%c|",FIXR01_ptr->investor_flag);
		fprintf(fptr, "Side=%d|",FIXR01_ptr->Side);
		fprintf(fptr, "OrdType=%d|",FIXR01_ptr->OrdType);
		fprintf(fptr, "TimeInForce=%u|",FIXR01_ptr->TimeInForce);
		fprintf(fptr, "PositionEffect=%c|",FIXR01_ptr->PositionEffect);	
        fprintf(fptr, "order_source=%c|",FIXR01_ptr->order_source);
        fprintf(fptr, "info_source=%c%c%c|",FIXR01_ptr->info_source[0], FIXR01_ptr->info_source[1], FIXR01_ptr->info_source[2]);	
        fprintf(fptr, "CheckSum=%03u",FIXR01_ptr->CheckSum);
		fprintf(fptr, "\n");
	}
    else if(type == FIXMsgType_R09)
	{
		FIX_R09_t* FIXR09_ptr = (FIX_R09_t*)data;
		time_t msg_time_sec;
		
		// fprintf(fptr, "Taifex R09:\n");
		fprintf(fptr, "msg_length=%d|", FIXGET_UINT16(FIXR09_ptr->MsgHeader.msg_length));
		fprintf(fptr, "MsgSeqNum=%d|", FIXGET_UINT32(FIXR09_ptr->MsgHeader.MsgSeqNum));
		fprintf(fptr, "MessageType=%d|", FIXGET_UINT8(FIXR09_ptr->MsgHeader.MessageType));
		msg_time_sec = FIXGET_UINT32(FIXR09_ptr->MsgHeader.SendingTime.epoch_s);
		fprintf(fptr, "SendingTime=%s|", GetTimeStr(msg_time_sec, FIXGET_UINT16(FIXR09_ptr->MsgHeader.SendingTime.ms)));
		//fprintf(fptr, "TargetCompID=%d|", FIXGET_UINT16(FIXR09_ptr->MsgHeader.TargetCompID));
		fprintf(fptr, "session_id=%d|", FIXGET_UINT16(FIXR09_ptr->MsgHeader.session_id));
		
		fprintf(fptr, "ExecType=%c|",FIXR09_ptr->ExecType);
		fprintf(fptr, "cm_id=%d|",FIXGET_UINT16(FIXR09_ptr->cm_id));
		fprintf(fptr, "TargetCompID=%d|",FIXGET_UINT16(FIXR09_ptr->TargetCompID));
		fprintf(fptr, "order_no=");
		for(i = 0; i < FIX_ORDNO_LEN; i++)
			fprintf(fptr, "%c",FIXR09_ptr->order_no[i]);
		fprintf(fptr, "|");	
		fprintf(fptr, "ord_id=%d|",FIXGET_UINT32(FIXR09_ptr->ord_id));
		fprintf(fptr, "user_define=");
		for(i = 0; i < FIX_UDD_LEN; i++)	
			fprintf(fptr, "%c",FIXR09_ptr->user_define[i]);
		fprintf(fptr, "|");
		fprintf(fptr, "symbol_type=%d|",FIXR09_ptr->symbol_type);	
		fprintf(fptr, "sym=");
		for(i = 0; i < FIX_SYM_LEN; i++)
			fprintf(fptr, "%c",FIXR09_ptr->sym[i]);
		fprintf(fptr, "|");
		fprintf(fptr, "BidPx=%d|",FIXGET_INT32(FIXR09_ptr->BidPx));
        fprintf(fptr, "OfferPx=%d|",FIXGET_INT32(FIXR09_ptr->OfferPx));
        fprintf(fptr, "BidSize=%d|",FIXGET_UINT16(FIXR09_ptr->BidSize));
        fprintf(fptr, "OfferSize=%d|",FIXGET_UINT16(FIXR09_ptr->OfferSize));
		fprintf(fptr, "investor_acno=%d|",FIXGET_UINT32(FIXR09_ptr->investor_acno));
		fprintf(fptr, "investor_flag=%c|",FIXR09_ptr->investor_flag);
		fprintf(fptr, "TimeInForce=%u|",FIXR09_ptr->TimeInForce);
		fprintf(fptr, "PositionEffect=%c|",FIXR09_ptr->PositionEffect);	
        fprintf(fptr, "order_source=%c|",FIXR09_ptr->order_source);
        fprintf(fptr, "info_source=%c%c%c|",FIXR09_ptr->info_source[0], FIXR09_ptr->info_source[1], FIXR09_ptr->info_source[2]);	
        fprintf(fptr, "CheckSum=%03u",FIXR09_ptr->CheckSum);
		fprintf(fptr, "\n");
	}
    else if(type == FIXMsgType_R04 || type == FIXMsgType_R05)
    {
        FIX_R05_t* FIXR05_ptr = (FIX_R05_t*)data;
		time_t msg_time_sec;
        // fprintf(fptr, "Taifex HB:\n");
        fprintf(fptr, "msg_length=%d|", FIXGET_UINT16(FIXR05_ptr->MsgHeader.msg_length));
		fprintf(fptr, "MsgSeqNum=%d|", FIXGET_UINT32(FIXR05_ptr->MsgHeader.MsgSeqNum));
		fprintf(fptr, "MessageType=%d|", FIXGET_UINT8(FIXR05_ptr->MsgHeader.MessageType));
		msg_time_sec = FIXGET_UINT32(FIXR05_ptr->MsgHeader.SendingTime.epoch_s);
		fprintf(fptr, "SendingTime=%s|", GetTimeStr(msg_time_sec, FIXGET_UINT16(FIXR05_ptr->MsgHeader.SendingTime.ms)));
		//fprintf(fptr, "TargetCompID=%d|", FIXGET_UINT16(FIXR05_ptr->MsgHeader.TargetCompID));
		fprintf(fptr, "session_id=%d|", FIXGET_UINT16(FIXR05_ptr->MsgHeader.session_id));
        fprintf(fptr, "status_code=%d|", FIXGET_UINT16(FIXR05_ptr->status_code));
        fprintf(fptr, "CheckSum=%03u",FIXR05_ptr->CheckSum);
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
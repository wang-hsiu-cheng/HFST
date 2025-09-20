#include "TaifexOrderUtility.h"

// void TaifexOrderUtility::FIXHdrSet(FIXhdr_t* hdr, uint16_t msgType, uint16_t msgLen)
// {
//     FIXSET_MSGLEN(hdr->msg_length, msgLen);
// 	FIXSET_UINT8(hdr->MessageType, msgType);
// 	FIXSET_INT32(hdr->SendingTime.epoch_s, time(NULL));
// }

std::string TaifexOrderUtility::FIXComputeCheckSum(const std::string& buf) {
    unsigned int cks = 0;

    for (char ch : buf)
        cks += static_cast<unsigned int>(ch);
    unsigned int checksumValue = cks % 256;

    std::ostringstream oss;
    oss << std::setw(3) << std::setfill('0') << checksumValue;

    return oss.str();
}

int TaifexOrderUtility::GetMsgLen(FIXhdr_t* hdr)
{
    std::string type = hdr->MessageType;
    int len = 0;
    // switch(type)
    // {
    //     case FIXMsgType_A: len = sizeof(FIX_A_t); break;
    //     case FIXMsgType_0: len = sizeof(FIX_0_t); break;
    //     case FIXMsgType_1: len = sizeof(FIX_1_t); break;
    //     case FIXMsgType_2: len = sizeof(FIX_2_t); break;
    //     case FIXMsgType_3: len = sizeof(FIX_3_t); break;
    //     case FIXMsgType_4: len = sizeof(FIX_4_t); break;
    //     case FIXMsgType_5: len = sizeof(FIX_5_t); break;
    //     case FIXMsgType_D: len = sizeof(FIX_D_t); break;
    //     case FIXMsgType_F: len = sizeof(FIX_F_t); break;
    //     case FIXMsgType_G: len = sizeof(FIX_G_t); break;
    //     case FIXMsgType_H: len = sizeof(FIX_H_t); break;
    //     case FIXMsgType_8: len = sizeof(FIX_8_t); break;
    //     case FIXMsgType_9: len = sizeof(FIX_9_t); break;
    //     case FIXMsgType_j: len = sizeof(FIX_j_t); break;
    // };

    return len;
}

std::string TaifexOrderUtility::GetMsg_time_tStr(Msg_time_t t)
{
	// Expected format: YYYYMMDD-HH:MM:SS.sss
    sprintf(dateString,
        "%04d%04d-%02d:%02d:%2.3f",
        t.YYYY,
        t.MMDD,
		t.HH,
		t.MM,
		t.SSsss);
    return dateString;
}

Msg_time_t TaifexOrderUtility::GetMsg_time_t()
{
	Msg_time_t t;

	auto now = std::chrono::system_clock::now();
	std::time_t now_t = std::chrono::system_clock::to_time_t(now);
	std::tm* local_tm = std::localtime(&now_t);

	t.YYYY = local_tm->tm_year;
	t.MMDD = (local_tm->tm_mon + 1) * 100 + local_tm->tm_mday;
	t.HH = local_tm->tm_hour;
	t.MM = local_tm->tm_min;
	t.SSsss = (float)local_tm->tm_sec;

	return t;
}

Msg_time_t TaifexOrderUtility::GetMsg_time_t(const std::string& timeStr)
{
	Msg_time_t t{};
    // Expected format: YYYYMMDD-HH:MM:SS.sss

    t.YYYY = static_cast<uint16_t>(std::stoi(timeStr.substr(0, 4)));
    t.MMDD = static_cast<uint16_t>(std::stoi(timeStr.substr(4, 4)));
    t.HH   = static_cast<uint8_t>(std::stoi(timeStr.substr(9, 2)));
    t.MM   = static_cast<uint8_t>(std::stoi(timeStr.substr(12, 2)));

    // Seconds and milliseconds
    int seconds = std::stoi(timeStr.substr(15, 2));
    int millis  = std::stoi(timeStr.substr(18, 3));
    t.SSsss = static_cast<float>(seconds + millis / 1000);

    return t;
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
	std::string type;
	FIXhdr_t *phdr = (FIXhdr_t*)data;
	
	type = phdr->MessageType;

	if(type == order.FIXMsgType["ExecutionReport"])
	{
		FIX_8_t* FIX8_ptr = (FIX_8_t*)data;
		time_t msg_time_sec;
		time_t org_trans_msg_time_sec;
		
		// fprintf(fptr, "Taifex R02:\n");
		fprintf(fptr, "msg_length=%d|", STR_TO_INT(FIX8_ptr->hdr.BodyLength));
		fprintf(fptr, "MsgSeqNum=%d|", FIX8_ptr->hdr.MsgSeqNum);
		fprintf(fptr, "MessageType=%s|", FIX8_ptr->hdr.MessageType.c_str());
		msg_time_sec = FIX8_ptr->hdr.SendingTime.SSsss;
		fprintf(fptr, "SendingTime=%f|", FIX8_ptr->hdr.SendingTime.SSsss);
		//fprintf(fptr, "TargetCompID=%d|", FIXGET_UINT16(FIXR02_ptr->MsgHeader.TargetCompID));
		fprintf(fptr, "session_id=%s|", FIX8_ptr->hdr.SenderCompID.c_str());

		fprintf(fptr, "ExecType=%c|",FIX8_ptr->ExecType);
		fprintf(fptr, "TargetCompID=%s|", FIX8_ptr->hdr.TargetCompID.c_str());
		fprintf(fptr, "order_no=");
		// for(i = 0; i < FIX_ORDNO_LEN; i++)
		fprintf(fptr, "%s",FIX8_ptr->ClOrdID.c_str());
		fprintf(fptr, "|");
		fprintf(fptr, "CheckSum=%03u",FIX8_ptr->CheckSum);
		fprintf(fptr, "\n");
	}
	else if(type == order.FIXMsgType["OrderCancelReject"])
	{
		FIX_9_t* FIX9_ptr = (FIX_9_t*)data;
		
		// fprintf(fptr, "Taifex R03:\n");
		fprintf(fptr, "msg_length=%d|", STR_TO_INT(FIX9_ptr->hdr.BodyLength));
		fprintf(fptr, "MsgSeqNum=%d|", FIX9_ptr->hdr.MsgSeqNum);
		fprintf(fptr, "MessageType=%s|", FIX9_ptr->hdr.MessageType.c_str());
		msg_time_sec = FIX9_ptr->hdr.SendingTime.SSsss;
		fprintf(fptr, "SendingTime=%f|", FIX9_ptr->hdr.SendingTime.SSsss);
		//fprintf(fptr, "TargetCompID=%d|", FIXGET_UINT16(FIXR03_ptr->MsgHeader.TargetCompID));
		fprintf(fptr, "session_id=%s|", FIX9_ptr->hdr.SenderCompID.c_str());
		
		// fprintf(fptr, "status_code=%d|",FIX9_ptr->OrderID);
		// fprintf(fptr, "ExecType=%c|",FIX9_ptr->ExecType);
		fprintf(fptr, "TargetCompID=%s|",FIX9_ptr->hdr.TargetCompID.c_str());
		fprintf(fptr, "CheckSum=%03u",FIX9_ptr->CheckSum);
		fprintf(fptr, "\n");
	}
    // else if(type == FIXMsgType_R01)
	// {
	// 	FIX_R01_t* FIXR01_ptr = (FIX_R01_t*)data;
	// 	time_t msg_time_sec;
		
	// 	// fprintf(fptr, "Taifex R01:\n");
	// 	fprintf(fptr, "msg_length=%d|", FIXGET_UINT16(FIXR01_ptr->MsgHeader.msg_length));
	// 	fprintf(fptr, "MsgSeqNum=%d|", FIXGET_UINT32(FIXR01_ptr->MsgHeader.MsgSeqNum));
	// 	fprintf(fptr, "MessageType=%d|", FIXGET_UINT8(FIXR01_ptr->MsgHeader.MessageType));
	// 	msg_time_sec = FIXGET_UINT32(FIXR01_ptr->MsgHeader.SendingTime.epoch_s);
	// 	fprintf(fptr, "SendingTime=%s|", GetTimeStr(msg_time_sec, FIXGET_UINT16(FIXR01_ptr->MsgHeader.SendingTime.ms)));
	// 	//fprintf(fptr, "TargetCompID=%d|", FIXGET_UINT16(FIXR01_ptr->MsgHeader.TargetCompID));
	// 	fprintf(fptr, "session_id=%d|", FIXGET_UINT16(FIXR01_ptr->MsgHeader.session_id));
		
	// 	fprintf(fptr, "ExecType=%c|",FIXR01_ptr->ExecType);
	// 	fprintf(fptr, "cm_id=%d|",FIXGET_UINT16(FIXR01_ptr->cm_id));
	// 	fprintf(fptr, "TargetCompID=%d|",FIXGET_UINT16(FIXR01_ptr->TargetCompID));
	// 	fprintf(fptr, "order_no=");
	// 	for(i = 0; i < FIX_ORDNO_LEN; i++)
	// 		fprintf(fptr, "%c",FIXR01_ptr->order_no[i]);
	// 	fprintf(fptr, "|");	
	// 	fprintf(fptr, "ord_id=%d|",FIXGET_UINT32(FIXR01_ptr->ord_id));
	// 	fprintf(fptr, "user_define=");
	// 	for(i = 0; i < FIX_UDD_LEN; i++)	
	// 		fprintf(fptr, "%c",FIXR01_ptr->user_define[i]);
	// 	fprintf(fptr, "|");
	// 	fprintf(fptr, "symbol_type=%d|",FIXR01_ptr->symbol_type);	
	// 	fprintf(fptr, "sym=");
	// 	for(i = 0; i < FIX_SYM_LEN; i++)
	// 		fprintf(fptr, "%c",FIXR01_ptr->sym[i]);
	// 	fprintf(fptr, "|");
	// 	fprintf(fptr, "Price=%d|",FIXGET_INT32(FIXR01_ptr->Price));
	// 	fprintf(fptr, "qty=%u|",FIXGET_UINT16(FIXR01_ptr->qty));
	// 	fprintf(fptr, "investor_acno=%d|",FIXGET_UINT32(FIXR01_ptr->investor_acno));
	// 	fprintf(fptr, "investor_flag=%c|",FIXR01_ptr->investor_flag);
	// 	fprintf(fptr, "Side=%d|",FIXR01_ptr->Side);
	// 	fprintf(fptr, "OrdType=%d|",FIXR01_ptr->OrdType);
	// 	fprintf(fptr, "TimeInForce=%u|",FIXR01_ptr->TimeInForce);
	// 	fprintf(fptr, "PositionEffect=%c|",FIXR01_ptr->PositionEffect);	
    //     fprintf(fptr, "order_source=%c|",FIXR01_ptr->order_source);
    //     fprintf(fptr, "info_source=%c%c%c|",FIXR01_ptr->info_source[0], FIXR01_ptr->info_source[1], FIXR01_ptr->info_source[2]);	
    //     fprintf(fptr, "CheckSum=%03u",FIXR01_ptr->CheckSum);
	// 	fprintf(fptr, "\n");
	// }
    // else if(type == FIXMsgType_R09)
	// {
	// 	FIX_R09_t* FIXR09_ptr = (FIX_R09_t*)data;
	// 	time_t msg_time_sec;
		
	// 	// fprintf(fptr, "Taifex R09:\n");
	// 	fprintf(fptr, "msg_length=%d|", FIXGET_UINT16(FIXR09_ptr->MsgHeader.msg_length));
	// 	fprintf(fptr, "MsgSeqNum=%d|", FIXGET_UINT32(FIXR09_ptr->MsgHeader.MsgSeqNum));
	// 	fprintf(fptr, "MessageType=%d|", FIXGET_UINT8(FIXR09_ptr->MsgHeader.MessageType));
	// 	msg_time_sec = FIXGET_UINT32(FIXR09_ptr->MsgHeader.SendingTime.epoch_s);
	// 	fprintf(fptr, "SendingTime=%s|", GetTimeStr(msg_time_sec, FIXGET_UINT16(FIXR09_ptr->MsgHeader.SendingTime.ms)));
	// 	//fprintf(fptr, "TargetCompID=%d|", FIXGET_UINT16(FIXR09_ptr->MsgHeader.TargetCompID));
	// 	fprintf(fptr, "session_id=%d|", FIXGET_UINT16(FIXR09_ptr->MsgHeader.session_id));
		
	// 	fprintf(fptr, "ExecType=%c|",FIXR09_ptr->ExecType);
	// 	fprintf(fptr, "cm_id=%d|",FIXGET_UINT16(FIXR09_ptr->cm_id));
	// 	fprintf(fptr, "TargetCompID=%d|",FIXGET_UINT16(FIXR09_ptr->TargetCompID));
	// 	fprintf(fptr, "order_no=");
	// 	for(i = 0; i < FIX_ORDNO_LEN; i++)
	// 		fprintf(fptr, "%c",FIXR09_ptr->order_no[i]);
	// 	fprintf(fptr, "|");	
	// 	fprintf(fptr, "ord_id=%d|",FIXGET_UINT32(FIXR09_ptr->ord_id));
	// 	fprintf(fptr, "user_define=");
	// 	for(i = 0; i < FIX_UDD_LEN; i++)	
	// 		fprintf(fptr, "%c",FIXR09_ptr->user_define[i]);
	// 	fprintf(fptr, "|");
	// 	fprintf(fptr, "symbol_type=%d|",FIXR09_ptr->symbol_type);	
	// 	fprintf(fptr, "sym=");
	// 	for(i = 0; i < FIX_SYM_LEN; i++)
	// 		fprintf(fptr, "%c",FIXR09_ptr->sym[i]);
	// 	fprintf(fptr, "|");
	// 	fprintf(fptr, "BidPx=%d|",FIXGET_INT32(FIXR09_ptr->BidPx));
    //     fprintf(fptr, "OfferPx=%d|",FIXGET_INT32(FIXR09_ptr->OfferPx));
    //     fprintf(fptr, "BidSize=%d|",FIXGET_UINT16(FIXR09_ptr->BidSize));
    //     fprintf(fptr, "OfferSize=%d|",FIXGET_UINT16(FIXR09_ptr->OfferSize));
	// 	fprintf(fptr, "investor_acno=%d|",FIXGET_UINT32(FIXR09_ptr->investor_acno));
	// 	fprintf(fptr, "investor_flag=%c|",FIXR09_ptr->investor_flag);
	// 	fprintf(fptr, "TimeInForce=%u|",FIXR09_ptr->TimeInForce);
	// 	fprintf(fptr, "PositionEffect=%c|",FIXR09_ptr->PositionEffect);	
    //     fprintf(fptr, "order_source=%c|",FIXR09_ptr->order_source);
    //     fprintf(fptr, "info_source=%c%c%c|",FIXR09_ptr->info_source[0], FIXR09_ptr->info_source[1], FIXR09_ptr->info_source[2]);	
    //     fprintf(fptr, "CheckSum=%03u",FIXR09_ptr->CheckSum);
	// 	fprintf(fptr, "\n");
	// }
    // else if(type == FIXMsgType_R04 || type == FIXMsgType_R05)
    // {
    //     FIX_R05_t* FIXR05_ptr = (FIX_R05_t*)data;
	// 	time_t msg_time_sec;
    //     // fprintf(fptr, "Taifex HB:\n");
    //     fprintf(fptr, "msg_length=%d|", FIXGET_UINT16(FIXR05_ptr->MsgHeader.msg_length));
	// 	fprintf(fptr, "MsgSeqNum=%d|", FIXGET_UINT32(FIXR05_ptr->MsgHeader.MsgSeqNum));
	// 	fprintf(fptr, "MessageType=%d|", FIXGET_UINT8(FIXR05_ptr->MsgHeader.MessageType));
	// 	msg_time_sec = FIXGET_UINT32(FIXR05_ptr->MsgHeader.SendingTime.epoch_s);
	// 	fprintf(fptr, "SendingTime=%s|", GetTimeStr(msg_time_sec, FIXGET_UINT16(FIXR05_ptr->MsgHeader.SendingTime.ms)));
	// 	//fprintf(fptr, "TargetCompID=%d|", FIXGET_UINT16(FIXR05_ptr->MsgHeader.TargetCompID));
	// 	fprintf(fptr, "session_id=%d|", FIXGET_UINT16(FIXR05_ptr->MsgHeader.session_id));
    //     fprintf(fptr, "status_code=%d|", FIXGET_UINT16(FIXR05_ptr->status_code));
    //     fprintf(fptr, "CheckSum=%03u",FIXR05_ptr->CheckSum);
    //     fprintf(fptr, "\n");
    // }
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

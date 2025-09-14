#include "TaifexOrderUtility.h"

std::string TaifexOrderUtility::FIXComputeCheckSum(const std::string& _buf) {
    unsigned int ckecks = 0;

    for (char check : _buf)
        ckecks += static_cast<unsigned int>(check);
    unsigned int checksumValue = ckecks % 256;

    std::ostringstream oss;
    oss << std::setw(3) << std::setfill('0') << checksumValue;

    return oss.str();
}

std::string TaifexOrderUtility::GetMsg_time_tStr(Msg_time_t _t)
{
	// Expected format: YYYYMMDD-HH:MM:SS.sss
    sprintf(dateString,
        "%04d%04d-%02d:%02d:%2.3f",
        _t.YYYY,
        _t.MMDD,
		_t.HH,
		_t.MM,
		_t.SSsss);
    return dateString;
}

Msg_time_t TaifexOrderUtility::GetMsg_time_t()
{
	Msg_time_t t;

	auto now = std::chrono::system_clock::now();
	std::time_t tNow = std::chrono::system_clock::to_time_t(now);
	std::tm* tmLocal = std::localtime(&tNow);

	t.YYYY = tmLocal->tm_year;
	t.MMDD = (tmLocal->tm_mon + 1) * 100 + tmLocal->tm_mday;
	t.HH = tmLocal->tm_hour;
	t.MM = tmLocal->tm_min;
	t.SSsss = (float)tmLocal->tm_sec;

	return t;
}

Msg_time_t TaifexOrderUtility::GetMsg_time_t(const std::string& _time_str)
{
	Msg_time_t t{};
    // Expected format: YYYYMMDD-HH:MM:SS.sss

    t.YYYY = static_cast<uint16_t>(std::stoi(_time_str.substr(0, 4)));
    t.MMDD = static_cast<uint16_t>(std::stoi(_time_str.substr(4, 4)));
    t.HH   = static_cast<uint8_t>(std::stoi(_time_str.substr(9, 2)));
    t.MM   = static_cast<uint8_t>(std::stoi(_time_str.substr(12, 2)));

    // Seconds and milliseconds
    int seconds = std::stoi(_time_str.substr(15, 2));
    int millis  = std::stoi(_time_str.substr(18, 3));
    t.SSsss = static_cast<float>(seconds + millis / 1000);

    return t;
}

void TaifexOrderUtility::PrintFIXPkt(FILE* _fptr, const void* _data)
{
	time_t msgTimeSec;
	int i;
	std::string type;
	FIXhdr_t *phdr = (FIXhdr_t*)_data;
	
	type = phdr->MessageType;

	if (type == order.FIXMsgType["ExecutionReport"]) {
		FIX_8_t* FIX8_ptr = (FIX_8_t*)_data;
		time_t msgTimeSec;
		time_t orgTransMsgTimeSec;
		
		// fprintf(_fptr, "Taifex R02:\n");
		fprintf(_fptr, "msg_length=%d|", STR_TO_INT(FIX8_ptr->Header.BodyLength));
		fprintf(_fptr, "MsgSeqNum=%d|", FIX8_ptr->Header.MsgSeqNum);
		fprintf(_fptr, "MessageType=%s|", FIX8_ptr->Header.MessageType.c_str());
		msgTimeSec = FIX8_ptr->Header.SendingTime.SSsss;
		fprintf(_fptr, "SendingTime=%f|", FIX8_ptr->Header.SendingTime.SSsss);
		//fprintf(_fptr, "TargetCompID=%d|", FIXGET_UINT16(FIXR02_ptr->MsgHeader.TargetCompID));
		fprintf(_fptr, "session_id=%s|", FIX8_ptr->Header.SenderCompID.c_str());

		fprintf(_fptr, "ExecType=%c|",FIX8_ptr->ExecType);
		fprintf(_fptr, "TargetCompID=%s|", FIX8_ptr->Header.TargetCompID.c_str());
		fprintf(_fptr, "order_no=");
		// for(i = 0; i < FIX_ORDNO_LEN; i++)
		fprintf(_fptr, "%s",FIX8_ptr->ClOrdID.c_str());
		fprintf(_fptr, "|");
		fprintf(_fptr, "CheckSum=%s",FIX8_ptr->CheckSum.c_str());
		fprintf(_fptr, "\n");
	}
	else if (type == order.FIXMsgType["OrderCancelReject"]) {
		FIX_9_t* FIX9_ptr = (FIX_9_t*)_data;
		
		// fprintf(_fptr, "Taifex R03:\n");
		fprintf(_fptr, "msg_length=%d|", STR_TO_INT(FIX9_ptr->Header.BodyLength));
		fprintf(_fptr, "MsgSeqNum=%d|", FIX9_ptr->Header.MsgSeqNum);
		fprintf(_fptr, "MessageType=%s|", FIX9_ptr->Header.MessageType.c_str());
		msgTimeSec = FIX9_ptr->Header.SendingTime.SSsss;
		fprintf(_fptr, "SendingTime=%f|", FIX9_ptr->Header.SendingTime.SSsss);
		//fprintf(_fptr, "TargetCompID=%d|", FIXGET_UINT16(FIXR03_ptr->MsgHeader.TargetCompID));
		fprintf(_fptr, "session_id=%s|", FIX9_ptr->Header.SenderCompID.c_str());
		
		// fprintf(_fptr, "status_code=%d|",FIX9_ptr->OrderID);
		// fprintf(_fptr, "ExecType=%c|",FIX9_ptr->ExecType);
		fprintf(_fptr, "TargetCompID=%s|",FIX9_ptr->Header.TargetCompID.c_str());
		fprintf(_fptr, "CheckSum=%s",FIX9_ptr->CheckSum.c_str());
		fprintf(_fptr, "\n");
	}
	else {
		fprintf(_fptr, "xOrder Taifex Unknown Type Packet:\n");
        uint8_t *ptr = (uint8_t*)_data;
		for(i = 0; i < 64 * 3; i++)
		{
			fprintf(_fptr, "%02x", ptr[i]);
			if(i % 64 == 63)
				fprintf(_fptr, "\n");
		}
		fprintf(_fptr, "\n");
	}
}
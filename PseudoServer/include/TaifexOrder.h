#ifndef _TAIFEXORDER_H_
#define _TAIFEXORDER_H_

#include <arpa/inet.h> 
#include <cstdint>
#include <iostream>
#include <map>

#define FIX_UDD_LEN 8
#define FIX_SYM_LEN 20
#define FIX_ORDNO_LEN 5
#define SOH '\x01'

#define FIXSET_CHAR_N(DES, SRC, N) (memcpy((DES), (SRC), (N)))
#define FIXSET_UINT8(DES, SRC) ((DES) = (SRC))
#define FIXSET_INT8(DES, SRC) ((DES) = (SRC))
#define FIXSET_UINT16(DES, SRC) ((DES) = htons((SRC)))
#define FIXSET_INT16(DES, SRC) ((DES) = htons((SRC)))
#define FIXSET_UINT32(DES, SRC) ((DES) = htonl((SRC)))
#define FIXSET_INT32(DES, SRC) ((DES) = htonl((SRC)))
#define FIXSET_INT64(DES, SRC) ((DES) = Htonll((SRC)))
#define FIXSET_MSGLEN(DES, LEN) ((DES) = htons((LEN) - sizeof(unsigned short) - sizeof(unsigned char)))

#define FIXGET_MSGLEN(SRC) ((unsigned short)(ntohs(SRC)) + sizeof(unsigned short) + sizeof(unsigned char) - sizeof(FIXHdr_t))

#define FIXGET_INT8(SRC) ((char)(SRC))
// #define FIXGET_UINT8(SRC) ((unsigned char)(SRC))
#define FIXGET_UINT16(SRC) ((unsigned short)ntohs((SRC)))
#define FIXGET_INT16(SRC) ((short)ntohs((SRC)))
#define FIXGET_UINT32(SRC) ((unsigned int)ntohl((SRC)))
#define FIXGET_INT32(SRC) ((int)ntohl((SRC)))
#define FIXGET_INT64(SRC) ((long)Ntohll((SRC)))

#define STR_TO_INT(SRC) (std::stoi(SRC))

#ifndef Ntohll
#define Ntohll(x) (((long)(ntohl((int)((x << 32) >> 32))) << 32) | ntohl(((int)(x >> 32))))
#endif
#ifndef Htonll
#define Htonll(x) Ntohll(x)
#endif
#define FIX_L20_t FIX_t
#define FIX_L42_t FIX_t
#define FIX_L60_t FIX_t

#define FIX_UDD_LEN 8
#define FIX_SYM_LEN 20
#define FIX_ORDNO_LEN 5

	/* SymbolFlag     */
#define FIX_SYM_NUM 1
#define FIX_SYM_TEXT 2

	/* ExecType       */
#define FIX_EXEC 'F'
#define FIX_EXEC_NEW '6'

	/* Order_source */
#define FIX_ORDSRC "D"

	/* Info_source */
#define FIX_INFOSRC "999"

	/* R11 broker->Taifex , order status query */
#define FIX_REQ_ORDSTATUS 0  /* query current status by group id. */
#define FIX_REG_ORDSTATUS 1  /* regist order status change event by group id. */
#define FIX_UREG_ORDSTATUS 2 /* unregist order status change event by group id. */

#define MAX_SIZE	65536

enum class State {
    Logging,
    Ordering,
    SendingLogout,
    Idle
};

enum class OrderState {
    Idle,
    SendExecutionReport,
    SendCancelReject
};

enum class SessionRejectStatus {
  BadFormat = 0,
  ValueOutOfBound = 1,
  MissingFields = 2,
  UnknownField = 4,
  FieldEmpty = 5,
  FieldOutOfOrder = 6,
  InvalidTagNumber = 10,
  NonRawDataHasSOH = 11,
  MessageMalformed = 100,
  MessageTypeUnknown = 101,
  MissingCompID = 110,
  IncorrectDataLength = 114,
  OptSessionRuleViolated = 201,
  SessionLogonBlocked = 204,
  SessionOnHold = 205
};

struct TaifexOrder{
    std::map<std::string, std::string> FIXMsgType = {
        {"Logon", "A"}, {"Heartbeat", "0"}, {"TestRequest", "1"}, {"ResendRequest", "2"}, {"RejectSession", "3"}, 
        {"SequenceReset", "4"}, {"Logout", "5"}, {"NewOrder", "D"}, {"OrderCancelRequest", "F"}, {"OrderCancel", "G"}, 
        {"OrderStatus", "H"}, {"ExecutionReport", "8"}, {"OrderCancelReject", "9"}, {"BusinessMessageReject", "j"}
    };
};

#pragma pack(1)
typedef struct _Msg_time_t
{
	uint16_t YYYY;
	uint16_t MMDD;
    uint8_t HH;
    uint8_t MM;
    float SSsss;
} Msg_time_t;
#pragma pack()

#pragma pack(1)
typedef struct _FIXhdr_t
{
	std::string BeginString;             // 8
	std::string BodyLength;              // 9
	std::string MessageType;             // 35
	uint32_t MsgSeqNum;                  // 34
	std::string SenderCompID = "T116001";// 49
	std::string TargetCompID = "XTAI";   // 56
	Msg_time_t SendingTime;              // 52
} FIXhdr_t;
#pragma pack()

#pragma pack(1)
typedef struct _FIX_A_t
{
    FIXhdr_t hdr;
    uint16_t EncryptMethod;              // 98
    uint16_t HeartBtInt = 10;            // 108
    uint16_t RawDataLength;              // 95
    uint16_t RawData;                    // 96
    std::string CheckSum;                // 10
} FIX_A_t;
#pragma pack()

#pragma pack(1)
typedef struct _FIX_0_t
{
    FIXhdr_t hdr;
    std::string TestReqID;              // 112
    unsigned char CheckSum;             // 10
} FIX_0_t;
#pragma pack()

#pragma pack(1)
typedef struct _FIX_1_t
{
    FIXhdr_t hdr;
    std::string TestReqID;              // 112
    unsigned char CheckSum;             // 10
} FIX_1_t;
#pragma pack()

#pragma pack(1)
typedef struct _FIX_2_t
{
    FIXhdr_t hdr;
    uint16_t BeginSeqNo;              // 7
    uint16_t EndSeqNo;                // 16
    unsigned char CheckSum;           // 10
} FIX_2_t;
#pragma pack()

#pragma pack(1)
typedef struct _FIX_3_t
{
    FIXhdr_t hdr;
    uint16_t RefSeqNum;              // 45
    uint16_t RefTagID;               // 371
    std::string RefMsgType;          // 372
    uint16_t SessionRejectReason;    // 373
    std::string Text;                // 58
    unsigned char CheckSum;          // 10
} FIX_3_t;
#pragma pack()

#pragma pack(1)
typedef struct _FIX_4_t
{
    FIXhdr_t hdr;
    bool GapFillFlag = true;         // 123
    uint16_t NewSeqNo;               // 36
    unsigned char CheckSum;          // 10
} FIX_4_t;
#pragma pack()

#pragma pack(1)
typedef struct _FIX_5_t
{
    FIXhdr_t hdr;
    std::string Text;                // 58
    unsigned char CheckSum;          // 10
} FIX_5_t;
#pragma pack()

#pragma pack(1)
typedef struct _FIX_D_t
{
    FIXhdr_t hdr;
	std::string ClOrdID;             // 11
	std::string OrderID;             // 37
    std::string Account;             // 1
    std::string Symbol;              // 55
    char Side;                       // 54
    Msg_time_t TransactTime;         // 60
    float OrderQty;                  // 38
    char OrderType;                  // 40
    char TimeInFource;               // 59
    float Price;                     // 44
    char RefOrderID;                 // 1080
    char TwselvacnoFlag;             // 10000
    char TwseOrdType;                // 10001
    char TwseExCode;                 // 10002
    bool TwseRejStaleOrd;            // 10004
    unsigned char CheckSum;          // 10
} FIX_D_t;
#pragma pack()

#pragma pack(1)
typedef struct _FIX_F_t
{
    FIXhdr_t hdr;
    std::string ClOrdID;             // 11
    std::string OrigClOrdID;         // 41
    std::string OrderID;             // 37
    std::string Account;             // 1
    std::string Symbol;              // 55
    char Side;                       // 54
    Msg_time_t TransactTime;         // 60
    char TwselvacnoFlag;             // 10000
    char TwseExCode;                 // 10002
    bool TwseRejStaleOrd;            // 10004
    unsigned char CheckSum;          // 10
} FIX_F_t;
#pragma pack()

#pragma pack(1)
typedef struct _FIX_G_t
{
    FIXhdr_t hdr;
    std::string ClOrdID;             // 11
    std::string OrigClOrdID;         // 41
    std::string OrderID;             // 37
    std::string Account;             // 1
    std::string Symbol;              // 55
    char Side;                       // 54
    Msg_time_t TransactTime;         // 60
    float OrderQty;                  // 38 (改量需填，不改量填 0)
    float Price;                     // 44 (改價需填，不改價填 0)
    char TwselvacnoFlag;             // 10000
    char TwseOrdType;                // 10001
    char TwseExCode;                 // 10002
    unsigned char CheckSum;          // 10
} FIX_G_t;
#pragma pack()

#pragma pack(1)
typedef struct _FIX_H_t
{
    FIXhdr_t hdr;
    std::string ClOrdID;             // 11
    std::string OrderID;             // 37
    std::string Symbol;              // 55
    char Side;                       // 54
    char TwselvacnoFlag;             // 10000
    char TwseExCode;                 // 10002
    unsigned char CheckSum;          // 10
} FIX_H_t;
#pragma pack()

#pragma pack(1)
typedef struct _FIX_8_t
{
    FIXhdr_t hdr;
    std::string OrderID;             // 37
    std::string ClOrdID;             // 11
    std::string OrigClOrdID;         // 41
    std::string ExecID;              // 17
    char ExecType;                   // 150
    char OrdStatus;                  // 39
    uint16_t OrdRejReason;           // 103
    uint16_t ExecRestatementReason;  // 378
    std::string Account;             // 1
    std::string Symbol;              // 55
    char Side;                       // 54
    Msg_time_t TransactTime;         // 60
    float OrderQty;                  // 38
    char OrdType;                    // 40
    char TimeInForce;                // 59
    float Price;                     // 44
    float LastQty;                   // 32
    float LastPx;                    // 31
    float LeavesQty;                 // 151
    float CumQty;                    // 14
    float AvgPx;                     // 6
    std::string Text;                // 58
    char RefOrderID;                 // 1080
    char TwseIvacnoFlag;             // 10000
    char TwseOrdType;                // 10001
    char TwseExCode;                 // 10002
    unsigned char CheckSum;          // 10
} FIX_8_t;
#pragma pack()

#pragma pack(1)
typedef struct _FIX_9_t
{
    FIXhdr_t hdr;
    std::string OrderID;             // 37
    std::string ClOrdID;             // 11
    std::string OrigClOrdID;         // 41
    char OrdStatus;                  // 39
    std::string Account;             // 1
    Msg_time_t TransactTime;         // 60
    char CxlRejResponseTo;           // 434
    uint8_t CxlRejReason;            // 102
    std::string Text;                // 58
    unsigned char CheckSum;          // 10
} FIX_9_t;
#pragma pack()

#pragma pack(1)
typedef struct _FIX_j_t
{
    FIXhdr_t hdr;
    uint32_t RefSeqNum;              // 45
    std::string RefMsgType;          // 372
    uint8_t BusinessRejectReason;    // 380
    std::string Text;                // 58
    unsigned char CheckSum;          // 10
} FIX_j_t;
#pragma pack()

#pragma pack(1)
typedef struct _FIX_MsgTime_t
{
	int epoch_s;	   /* second from Epoch(00:00:00 UTC 1970/1/1) */
	unsigned short ms; /* millisecond */
} FIX_MsgTime_t;
#pragma pack()

#endif
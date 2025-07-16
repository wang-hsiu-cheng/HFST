#ifndef _TAIFEXORDER_H_
#define _TAIFEXORDER_H_

#include <arpa/inet.h> 
#include <cstdint>
#include <map>

#define FIX_UDD_LEN 8
#define FIX_SYM_LEN 20
#define FIX_ORDNO_LEN 5

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
#define FIXGET_UINT8(SRC) ((unsigned char)(SRC))
#define FIXGET_UINT16(SRC) ((unsigned short)ntohs((SRC)))
#define FIXGET_INT16(SRC) ((short)ntohs((SRC)))
#define FIXGET_UINT32(SRC) ((unsigned int)ntohl((SRC)))
#define FIXGET_INT32(SRC) ((int)ntohl((SRC)))
#define FIXGET_INT64(SRC) ((long)Ntohll((SRC)))

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
    Loggingout
};

// Event Enum
enum class Event {
    Logon,
    Order,
    Logout
};

// enum class OrderState {
//     TestRequest,
//     NewOrder,
//     ExecutionReport
// };

#pragma pack(1)
typedef struct _FIXMsgType
{
	char Logon = 'A',
	char Heartbeat = '0',
	char TestRequest = '1',
	char ResendRequest = '2',
	char RejectSession = '3',
	char SequenceReset = '4',
	char Logout = '5',
	char NewOrder = 'D',
	char OrderCancel = 'G',
	char OrderCancelRequest = 'F',
	char OrderStatus = 'H',
	char ExecutionReport = '8',
	char OrderCancelReject = '9'
} FIXMsgType;

#pragma pack(1)
typedef struct _Msg_time_t
{
	int32_t epoch_s;
	uint16_t ms;
} Msg_time_t;
#pragma pack()

#pragma pack(1)
typedef struct _FIXhdr_t
{
	std::string BeginString;             // 8
	uint16_t BodyLength;                 // 9
	uint8_t MessageType;                 // 35
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
    unsigned char CheckSum;              // 10
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
    std::string RefTagID;            // 371
    char RefMsgType;                 // 372
    std::string SessionRejectReason; // 373
    std::string Text;                // 58
    unsigned char CheckSum;          // 10
} FIX_3_t;
#pragma pack()

#pragma pack(1)
typedef struct _FIX_4_t
{
    FIXhdr_t hdr;
    char GapFillFlag = 'Y';          // 123
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
    uint16_t Symbol;                 // 55
    uint16_t Side;                   // 54
    Msg_time_t TransactTime;         // 60
    uint16_t OrderQty;               // 38
    uint16_t OrderType;              // 40
    uint16_t TimeInFource;           // 59
    uint16_t Price;                  // 44
    char TwselvacnoFlag;             // 10000
    char TwseOrdType;                // 10001
    char TwseExCode;                 // 10002
    char TwseRejStaleOrd;            // 10004
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
    uint16_t Symbol;                 // 55
    uint16_t Side;                   // 54
    Msg_time_t TransactTime;         // 60
    char TwselvacnoFlag;             // 10000
    char TwseOrdType;                // 10001
    char TwseExCode;                 // 10002
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
    uint16_t Symbol;                 // 55
    uint16_t Side;                   // 54
    Msg_time_t TransactTime;         // 60
    uint16_t OrderQty;               // 38 (改量需填，不改量填 0)
    uint16_t Price;                  // 44 (改價需填，不改價填 0)
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
    std::string Account;             // 1
    uint16_t Symbol;                 // 55
    uint16_t Side;                   // 54
    char TwselvacnoFlag;             // 10000
    char TwseOrdType;                // 10001
    char TwseExCode;                 // 10002
    unsigned char CheckSum;          // 10
} FIX_H_t;
#pragma pack()

#pragma pack(1)
typedef struct _FIX_8_t
{
    FIXhdr_t hdr;
    std::string ClOrdID;             // 11
    std::string OrderID;             // 37
    std::string ExecID;              // 17
    uint8_t ExecTransType;           // 20
    uint8_t ExecType;                // 150
    uint8_t OrdStatus;               // 39
    uint16_t Symbol;                 // 55
    uint16_t Side;                   // 54
    uint16_t OrderQty;               // 38
    uint16_t LeavesQty;              // 151
    uint16_t CumQty;                 // 14
    uint16_t LastQty;                // 32
    uint16_t AvgPx;                  // 6
    Msg_time_t TransactTime;         // 60
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
    uint8_t OrdStatus;               // 39
    uint8_t CxlRejResponseTo;        // 434
    uint8_t CxlRejReason;            // 102
    std::string Text;                // 58
    unsigned char CheckSum;          // 10
} FIX_9_t;
#pragma pack()

#pragma pack(1)
typedef struct _FIX_MsgTime_t
{
	int epoch_s;	   /* second from Epoch(00:00:00 UTC 1970/1/1) */
	unsigned short ms; /* millisecond */
} FIX_MsgTime_t;
#pragma pack()

#endif
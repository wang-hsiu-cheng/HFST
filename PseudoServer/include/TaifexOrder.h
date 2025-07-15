#ifndef _TAIFEXORDER_H_
#define _TAIFEXORDER_H_

#include <arpa/inet.h> 
#include <cstdint>

#define TMP_UDD_LEN 8
#define TMP_SYM_LEN 20
#define TMP_ORDNO_LEN 5

#define TMPSET_CHAR_N(DES, SRC, N) (memcpy((DES), (SRC), (N)))
#define TMPSET_UINT8(DES, SRC) ((DES) = (SRC))
#define TMPSET_INT8(DES, SRC) ((DES) = (SRC))
#define TMPSET_UINT16(DES, SRC) ((DES) = htons((SRC)))
#define TMPSET_INT16(DES, SRC) ((DES) = htons((SRC)))
#define TMPSET_UINT32(DES, SRC) ((DES) = htonl((SRC)))
#define TMPSET_INT32(DES, SRC) ((DES) = htonl((SRC)))
#define TMPSET_INT64(DES, SRC) ((DES) = Htonll((SRC)))
#define TMPSET_MSGLEN(DES, LEN) ((DES) = htons((LEN) - sizeof(unsigned short) - sizeof(unsigned char)))

#define TMPGET_MSGLEN(SRC) ((unsigned short)(ntohs(SRC)) + sizeof(unsigned short) + sizeof(unsigned char) - sizeof(TMPHdr_t))

#define TMPGET_INT8(SRC) ((char)(SRC))
#define TMPGET_UINT8(SRC) ((unsigned char)(SRC))
#define TMPGET_UINT16(SRC) ((unsigned short)ntohs((SRC)))
#define TMPGET_INT16(SRC) ((short)ntohs((SRC)))
#define TMPGET_UINT32(SRC) ((unsigned int)ntohl((SRC)))
#define TMPGET_INT32(SRC) ((int)ntohl((SRC)))
#define TMPGET_INT64(SRC) ((long)Ntohll((SRC)))

#ifndef Ntohll
#define Ntohll(x) (((long)(ntohl((int)((x << 32) >> 32))) << 32) | ntohl(((int)(x >> 32))))
#endif
#ifndef Htonll
#define Htonll(x) Ntohll(x)
#endif
#define TMP_L20_t TMPRTN_t
#define TMP_L42_t TMPRTN_t
#define TMP_L60_t TMPRTN_t

#define TMP_UDD_LEN 8
#define TMP_SYM_LEN 20
#define TMP_ORDNO_LEN 5

	/* SymbolFlag     */
#define TMP_SYM_NUM 1
#define TMP_SYM_TEXT 2

	/* ExecType       */
#define TMP_EXEC 'F'
#define TMP_EXEC_NEW '6'

	/* Order_source */
#define TMP_ORDSRC "D"

	/* Info_source */
#define TMP_INFOSRC "999"

	/* R11 broker->Taifex , order status query */
#define TMP_REQ_ORDSTATUS 0  /* query current status by group id. */
#define TMP_REG_ORDSTATUS 1  /* regist order status change event by group id. */
#define TMP_UREG_ORDSTATUS 2 /* unregist order status change event by group id. */

#define MAX_SIZE	65536

typedef enum _TMPMsgType
{
	TMPMsgType_L10 = 10,
	TMPMsgType_L20 = 20,
	TMPMsgType_L30 = 30,
	TMPMsgType_L40 = 40,
	TMPMsgType_L41 = 41,
	TMPMsgType_L42 = 42,
	TMPMsgType_L50 = 50,
	TMPMsgType_L60 = 60,
	TMPMsgType_R01 = 101,
	TMPMsgType_R02 = 102,
	TMPMsgType_R03 = 103,
	TMPMsgType_R04 = 104,
	TMPMsgType_R05 = 105,
	TMPMsgType_R07 = 107,
	TMPMsgType_R08 = 108,
	TMPMsgType_R09 = 109,
	TMPMsgType_R11 = 111,
	TMPMsgType_R12 = 112
} TMPMsgType;

#pragma pack(1)
typedef struct _Msg_time_t
{
	int32_t epoch_s;
	uint16_t ms;
} Msg_time_t;
#pragma pack()

#pragma pack(1)
typedef struct _TMPhdr_t
{
	uint16_t msg_length;
	uint32_t MsgSeqNum;
	Msg_time_t msg_time;//6
	uint8_t MessageType;
	uint16_t fcm_id;
	uint16_t session_id;
} TMPhdr_t;//17
#pragma pack()

#pragma pack(1)
typedef struct _TMPRTN_t
{
	TMPhdr_t MsgHeader;
	unsigned char status_code;
	unsigned char CheckSum;
} TMPRTN_t;
#pragma pack()

#pragma pack(1)
typedef struct _TMP_L10_t
{
	TMPhdr_t MsgHeader;
	uint8_t status_code;
	uint32_t start_in_bound_num;
	uint8_t CheckSum;
} TMP_L10_t;
#pragma pack()

#pragma pack(1)
typedef struct _TMP_L30_t
{
	TMPhdr_t MsgHeader;
	uint8_t status_code;
	uint16_t append_no;
	uint32_t end_in_bound_num;
	uint8_t system_type;
	uint8_t EncryptMethod;
	uint8_t CheckSum;
} TMP_L30_t;
#pragma pack()
		
#pragma pack(1)
typedef struct _TMP_L40_t
{
	TMPhdr_t MsgHeader;
	uint8_t status_code;
	uint16_t append_no;
	uint16_t fcm_id;
	uint16_t session_id;
	uint8_t system_type;
	uint8_t ap_code;
	uint8_t key_value;
	uint32_t request_start_seq;
	uint8_t cancel_order_sec;
	uint8_t CheckSum;
} TMP_L40_t;
#pragma pack()

#pragma pack(1)
typedef struct _TMP_L41_t
{
	TMPhdr_t MsgHeader;
	uint8_t status_code;
	uint8_t is_eof;
	uint32_t file_size;
	char *data;
	uint8_t CheckSum;
} TMP_L41_t;
#pragma pack()
		
#pragma pack(1)
typedef struct _TMP_L50_t
{
	TMPhdr_t MsgHeader;
	uint8_t status_code;
	uint8_t HeartBtInt;
	uint16_t max_flow_ctrl_cnt;
	uint8_t CheckSum;
} TMP_L50_t;
#pragma pack()

#pragma pack(1)
typedef struct _TMP_R04_t
{
	TMPhdr_t MsgHeader;
	uint8_t status_code;
	uint8_t CheckSum;
} TMP_R04_t;
#pragma pack()

#pragma pack(1)
typedef struct _TMP_R05_t
{
	TMPhdr_t MsgHeader;
	uint8_t status_code;
	uint8_t CheckSum;
} TMP_R05_t;
#pragma pack()

#pragma pack(1)
typedef struct _TMP_R11_t
{
	TMPhdr_t MsgHeader;
	uint8_t status_code;
	uint16_t TradeReqID;
	uint8_t flow_group_no;
	uint8_t SubscriptionRequestType;
	uint8_t CheckSum;
} TMP_R11_t;
#pragma pack()

#pragma pack(1)
typedef struct _TMP_R12_t
{
	TMPhdr_t MsgHeader;
	uint8_t status_code;
	uint16_t TradeReqID;
	uint8_t flow_group_no;
	uint8_t TradeStatus;
	uint8_t CheckSum;
} TMP_R12_t;
#pragma pack()

#pragma pack(1)
typedef struct _TMP_MsgTime_t
{
	int epoch_s;	   /* second from Epoch(00:00:00 UTC 1970/1/1) */
	unsigned short ms; /* millisecond */
} TMP_MsgTime_t;
#pragma pack()

/* Order Request */
#pragma pack(1)
typedef struct _TMP_R01_t
{
	TMPhdr_t MsgHeader;
	char ExecType; /* 0:New, 4:Cxl, 5:Reduce Qty m:Modify Price */
	unsigned short cm_id;
	unsigned short fcm_id;
	char order_no[TMP_ORDNO_LEN];
	unsigned int ord_id;
	char user_define[TMP_UDD_LEN];
	unsigned char symbol_type; /* 2:text */
	char sym[TMP_SYM_LEN];
	int Price;
	unsigned short qty;
	unsigned int investor_acno;
	char investor_flag;
	unsigned char Side;		   /* 1:buy, 2:sell */
	unsigned char OrdType;	 /* 1:market, 2:limit */
	unsigned char TimeInForce; /* 4:FOK, 3:IOC 0:ROD */
	char PositionEffect;	   /* O:open, C:close, D:daytrade */
	char order_source;
	char info_source[3];
	unsigned char CheckSum;
} TMP_R01_t;
#pragma pack()

/* Order Request  R09 */
#pragma pack(1)
typedef struct _TMP_R09_t
{
	TMPhdr_t MsgHeader;
	char ExecType; /* 0:New, 4:Cxl, 5:Reduce Qty */
	unsigned short cm_id;
	unsigned short fcm_id;
	char order_no[TMP_ORDNO_LEN];
	unsigned int ord_id;
	char user_define[TMP_UDD_LEN];
	uint8_t symbol_type; /* 2:text */
	char sym[TMP_SYM_LEN];
	int BidPx;
	int OfferPx;
	unsigned short BidSize;
	unsigned short OfferSize;
	unsigned int investor_acno;
	char investor_flag;
	unsigned char TimeInForce; /* 0:ROD 8:auto-cxl */
	char PositionEffect;	   /* 9:market maker */
	char order_source;
	char info_source[3];
	unsigned char CheckSum;
} TMP_R09_t;
#pragma pack()

/* Order Report */
#pragma pack(1)
typedef struct _TMP_R02_t
{
	TMPhdr_t MsgHeader;
	unsigned char status_code;
	char ExecType; /* 0:New, 4:Cxl, 5:Reduce Qty m:Modify Price */
	unsigned short cm_id;
	unsigned short fcm_id;
	char order_no[TMP_ORDNO_LEN];
	unsigned int ord_id;
	char user_define[TMP_UDD_LEN];
	unsigned char symbol_type; /* 2:text */
	char sym[TMP_SYM_LEN];
	int Price;
	unsigned short qty;
	unsigned int investor_acno;
	char investor_flag;
	unsigned char Side;		   /* 1:buy, 2:sell */
	unsigned char OrdType;	 /* 1:market, 2:limit */
	unsigned char TimeInForce; /* 4:FOK, 3:IOC 0:ROD 8:Quote */
	char PositionEffect;	   /* 9:Market Maker */
	int LastPx;
	unsigned short LastQty;
	long px_subtotal;
	unsigned short CumQty;
	unsigned short LeavesQty;
	unsigned short before_qty;
	unsigned char leg_side_1;
	unsigned char leg_side_2;
	int leg_px_1;
	int leg_px_2;
	unsigned short leg_qty_1;
	unsigned short leg_qty_2;
	TMP_MsgTime_t org_trans_time;
	TMP_MsgTime_t TransactTime;
	unsigned char target_id;
	unsigned int uniq_id;
	unsigned int rpt_seq;
	unsigned char protocol_type;
	unsigned char CheckSum;
} TMP_R02_t;
#pragma pack()

/* Error Report */
#pragma pack(1)
typedef struct _TMP_R03_t
{
	TMPhdr_t MsgHeader;
	unsigned char status_code;
	char ExecType; /* 0:New, 4:Cxl, 5:Reduce Qty m:Modify Price */
	unsigned short fcm_id;
	char order_no[TMP_ORDNO_LEN];
	unsigned int ord_id;
	char user_define[TMP_UDD_LEN];
	unsigned int rpt_seq;
	unsigned char Side; /* 1:buy, 2:sell */
	unsigned char CheckSum;
} TMP_R03_t;
#pragma pack()

#pragma pack(1)
typedef struct _TMP_R07_t
{
    TMPhdr_t MsgHeader;
    char order_no[TMP_ORDNO_LEN];
    unsigned int ord_id;
	unsigned short fcm_id;
	unsigned char symbol_type; /* 2:text */
	char sym[TMP_SYM_LEN];
    char order_source;
	char info_source[3];
    unsigned char CheckSum;
} TMP_R07_t;
#pragma pack()

#pragma pack(1)
typedef struct _TMP_R08_t
{
    TMPhdr_t MsgHeader;
	uint8_t status_code;
    char order_no[TMP_ORDNO_LEN];
    unsigned int ord_id;
	unsigned short fcm_id;
	unsigned char symbol_type; /* 2:text */
	char sym[TMP_SYM_LEN];
    unsigned char CheckSum;
} TMP_R08_t;
#pragma pack()

#endif
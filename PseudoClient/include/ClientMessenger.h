#ifndef SERVERMESSENGER_H
#define SERVERMESSENGER_H

#include <iostream>
#include <vector>
#include <memory>
#include <cstring>
#include <ctime>
#include <cerrno>
#include <iomanip>
#include <time.h>

#include "TaifexOrder.h"
#include "TaifexOrderUtility.h"
#include "TaifexORGenerator.h"

using namespace std;

// Values the user puts into the "Order Out Register" (what the trading
// algorithm would hand to the FIX encoder on the FPGA).
struct OrderParams
{
    std::string ClOrdID;      // 11
    std::string OrigClOrdID;  // 41 (F/G)
    std::string Symbol;       // 55
    char Side = '1';          // 54  1=Buy 2=Sell
    double Qty = 0;           // 38
    double Price = 0;         // 44
};

// Decoded MarketDataSnapshotFullRefresh (35=W).
struct MDQuote
{
    std::string Symbol;
    double Bid = 0, BidSize = 0;
    double Ask = 0, AskSize = 0;
    double Last = 0, Volume = 0;
};

class ClientMessenger
{
public:
    string MakeA(const std::string& buf, FIXhdr_t header, uint16_t clientSeqNum);
    string Make0(const std::string& buf, FIXhdr_t header, uint16_t clientSeqNum);
    string Make1(const std::string& buf, FIXhdr_t header, uint16_t clientSeqNum);
    string Make2(FIX_2_t *msg, FIXhdr_t header, uint16_t clientSeqNum);
    string Make2resp(const std::string& buf, FIXhdr_t header, uint16_t clientSeqNum);
    string Make3(FIX_3_t *msg, FIXhdr_t header, uint16_t clientSeqNum);
    string Make3resp(const std::string& buf, FIXhdr_t header, uint16_t clientSeqNum);
    string Make4(const std::string& buf, FIXhdr_t header, uint16_t clientSeqNum);
    string Make5(const std::string& buf, FIXhdr_t header, uint16_t clientSeqNum);
    string MakeD(const std::string& buf, FIXhdr_t header, uint16_t clientSeqNum);
    string MakeF(const std::string& buf, FIXhdr_t header, uint16_t clientSeqNum);
    string MakeG(const std::string& buf, FIXhdr_t header, uint16_t clientSeqNum);
    string MakeH(const std::string& buf, FIXhdr_t header, uint16_t clientSeqNum);

    // Encoders driven by user parameters. Same framing as the legacy MakeX
    // (BodyLength = body size, CheckSum over everything before tag 10).
    string MakeD(const OrderParams& p, FIXhdr_t header, uint16_t clientSeqNum);
    string MakeF(const OrderParams& p, FIXhdr_t header, uint16_t clientSeqNum);
    string MakeG(const OrderParams& p, FIXhdr_t header, uint16_t clientSeqNum);
    string MakeH(const OrderParams& p, FIXhdr_t header, uint16_t clientSeqNum);

    string getChecksumFromFIX(const std::string& buf);
    int SearchMsgType(const std::string& buf, FIXhdr_t& header, FIX_2_t& resendRequest_msg, FIX_3_t& reject_msg);
    string DecodeMessage(const std::string &msg, std::string target_tag);
    bool ReceiveA(std::string &msg, FIX_A_t *logonMsg);
    bool ReceiveOrder(std::string &msg, std::string msgType);

    // Decoders: packet -> struct (no trading logic).
    bool Receive8(const std::string& msg, FIX_8_t& out);
    bool Receive9(const std::string& msg, FIX_9_t& out);
    bool ReceiveW(const std::string& msg, MDQuote& out);

    TaifexOrderUtility util;
    TaifexOrder order;
    TaifexORGenerator gen;
};

#endif

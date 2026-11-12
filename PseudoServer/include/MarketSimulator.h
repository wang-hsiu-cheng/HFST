#ifndef MARKET_SIMULATOR_H
#define MARKET_SIMULATOR_H

#include <map>
#include <mutex>
#include <random>
#include <string>
#include <vector>

// Fake quote for one symbol.
struct Quote
{
    std::string symbol;
    double refPx = 0;   // previous close, used for change %
    double bid = 0;
    double ask = 0;
    double last = 0;
    int bidSz = 0;
    int askSz = 0;
    long volume = 0;
};

// Generates a random-walk quote stream on TWSE tick sizes.
class MarketSimulator
{
public:
    explicit MarketSimulator(unsigned seed = std::random_device{}());

    void Tick();
    std::vector<Quote> Snapshot() const;
    bool Get(const std::string& symbol, Quote& out) const;
    // Accepts "2330" as well as zero-padded "002330".
    std::string Normalize(const std::string& symbol) const;
    void AddVolume(const std::string& symbol, double qty, double px);

    static double TickSize(double px);
    static double RoundToTick(double px);

private:
    void Requote(Quote& q);

    mutable std::mutex m_mutex;
    std::map<std::string, Quote> m_quotes;
    std::mt19937 m_rng;
};

// One simulated order resting at the exchange.
struct SimOrder
{
    std::string orderID;
    std::string clOrdID;      // latest ClOrdID for this order
    std::string symbol;
    char side = '1';          // '1' buy, '2' sell
    double qty = 0;
    double px = 0;
    double cumQty = 0;
    double avgPx = 0;
    char status = '0';        // FIX OrdStatus
};

// Result of a matching action, turned into a FIX 8 or 9 by the server.
struct ExecEvent
{
    bool isCancelReject = false;  // true -> MsgType 9, false -> MsgType 8
    char execType = '0';          // 150
    char cxlRejResponseTo = '1';  // 434 (only for 9)
    SimOrder order;               // snapshot after the event
    std::string clOrdID;          // 11 echoed back
    std::string origClOrdID;      // 41
    double lastQty = 0;
    double lastPx = 0;
    std::string text;
};

// Very small limit-order matcher against the simulated top of book.
class MatchingEngine
{
public:
    explicit MatchingEngine(MarketSimulator& market) : m_market(market) {}

    std::vector<ExecEvent> NewOrder(const std::string& clOrdID, const std::string& symbol,
                                    char side, double qty, double px);
    std::vector<ExecEvent> Cancel(const std::string& clOrdID, const std::string& origClOrdID);
    std::vector<ExecEvent> Replace(const std::string& clOrdID, const std::string& origClOrdID,
                                   double qty, double px);
    std::vector<ExecEvent> Status(const std::string& clOrdID);
    // Re-check resting orders against the latest quotes.
    std::vector<ExecEvent> OnTick();

    size_t OpenOrders() const;

private:
    bool TryFill(SimOrder& o, const std::string& clOrdID, std::vector<ExecEvent>& out);
    SimOrder* Find(const std::string& anyClOrdID);
    ExecEvent CancelReject(const std::string& clOrdID, const std::string& origClOrdID,
                           char responseTo, const std::string& text);

    MarketSimulator& m_market;
    mutable std::mutex m_mutex;
    std::map<std::string, SimOrder> m_orders;          // OrderID -> order
    std::map<std::string, std::string> m_clToOrder;    // every ClOrdID -> OrderID
    int m_nextOrderID = 1;
};

#endif

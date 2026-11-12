#include "MarketSimulator.h"

#include <cmath>
#include <cstdio>

/******************************** MarketSimulator ********************************/

MarketSimulator::MarketSimulator(unsigned seed) : m_rng(seed)
{
    const std::vector<std::pair<std::string, double>> symbols = {
        {"2330", 600.0}, {"2317", 110.0}, {"2454", 1000.0}, {"0050", 150.0}};
    for (const auto& s : symbols) {
        Quote q;
        q.symbol = s.first;
        q.refPx = s.second;
        q.last = s.second;
        Requote(q);
        m_quotes[s.first] = q;
    }
}

// TWSE price tick table.
double MarketSimulator::TickSize(double px)
{
    if (px < 10) return 0.01;
    if (px < 50) return 0.05;
    if (px < 100) return 0.1;
    if (px < 500) return 0.5;
    if (px < 1000) return 1.0;
    return 5.0;
}

double MarketSimulator::RoundToTick(double px)
{
    double t = TickSize(px);
    return std::round(px / t) * t;
}

void MarketSimulator::Requote(Quote& q)
{
    std::uniform_int_distribution<int> size(1, 50);
    q.last = RoundToTick(q.last);
    q.bid = q.last;
    q.ask = RoundToTick(q.last + TickSize(q.last));
    q.bidSz = size(m_rng) * 1000;
    q.askSz = size(m_rng) * 1000;
}

void MarketSimulator::Tick()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::discrete_distribution<int> move({1, 3, 8, 3, 1});  // -2..+2 ticks, mostly flat
    std::uniform_int_distribution<int> vol(0, 20);
    for (auto& kv : m_quotes) {
        Quote& q = kv.second;
        int steps = move(m_rng) - 2;
        double px = q.last + steps * TickSize(q.last);
        // keep within the +/-10% daily limit
        px = std::max(q.refPx * 0.9, std::min(q.refPx * 1.1, px));
        q.last = px;
        q.volume += vol(m_rng) * 1000;
        Requote(q);
    }
}

std::vector<Quote> MarketSimulator::Snapshot() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<Quote> out;
    for (const auto& kv : m_quotes)
        out.push_back(kv.second);
    return out;
}

std::string MarketSimulator::Normalize(const std::string& symbol) const
{
    if (m_quotes.count(symbol))
        return symbol;
    if (symbol.size() > 4 && m_quotes.count(symbol.substr(symbol.size() - 4)))
        return symbol.substr(symbol.size() - 4);
    return symbol;
}

bool MarketSimulator::Get(const std::string& symbol, Quote& out) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_quotes.find(Normalize(symbol));
    if (it == m_quotes.end())
        return false;
    out = it->second;
    return true;
}

void MarketSimulator::AddVolume(const std::string& symbol, double qty, double px)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_quotes.find(Normalize(symbol));
    if (it == m_quotes.end())
        return;
    it->second.volume += static_cast<long>(qty);
    it->second.last = px;
}

/******************************** MatchingEngine ********************************/

SimOrder* MatchingEngine::Find(const std::string& anyClOrdID)
{
    auto it = m_clToOrder.find(anyClOrdID);
    if (it == m_clToOrder.end())
        return nullptr;
    auto o = m_orders.find(it->second);
    return o == m_orders.end() ? nullptr : &o->second;
}

ExecEvent MatchingEngine::CancelReject(const std::string& clOrdID, const std::string& origClOrdID,
                                       char responseTo, const std::string& text)
{
    ExecEvent e;
    e.isCancelReject = true;
    e.cxlRejResponseTo = responseTo;
    e.clOrdID = clOrdID;
    e.origClOrdID = origClOrdID;
    e.text = text;
    e.order.clOrdID = clOrdID;
    e.order.orderID = "NONE";
    e.order.status = '8';
    SimOrder* o = Find(origClOrdID);
    if (o) {
        e.order = *o;
    }
    return e;
}

// Fill the whole order at the touch if the limit price crosses it.
bool MatchingEngine::TryFill(SimOrder& o, const std::string& clOrdID, std::vector<ExecEvent>& out)
{
    if (o.status != '0')
        return false;
    Quote q;
    if (!m_market.Get(o.symbol, q))
        return false;
    bool buy = o.side == '1';
    bool crosses = buy ? (o.px >= q.ask) : (o.px <= q.bid);
    if (!crosses)
        return false;
    double fillPx = buy ? q.ask : q.bid;
    double fillQty = o.qty - o.cumQty;
    o.avgPx = (o.avgPx * o.cumQty + fillPx * fillQty) / (o.cumQty + fillQty);
    o.cumQty += fillQty;
    o.status = '2';
    m_market.AddVolume(o.symbol, fillQty, fillPx);

    ExecEvent e;
    e.execType = 'F';
    e.order = o;
    e.clOrdID = clOrdID;
    e.lastQty = fillQty;
    e.lastPx = fillPx;
    e.text = "filled";
    out.push_back(e);
    return true;
}

std::vector<ExecEvent> MatchingEngine::NewOrder(const std::string& clOrdID, const std::string& symbol,
                                                char side, double qty, double px)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<ExecEvent> out;

    SimOrder o;
    char id[16];
    std::snprintf(id, sizeof(id), "O%06d", m_nextOrderID++);
    o.orderID = id;
    o.clOrdID = clOrdID;
    o.symbol = m_market.Normalize(symbol);
    o.side = side;
    o.qty = qty;
    o.px = px;

    std::string reason;
    Quote q;
    if (!m_market.Get(o.symbol, q))
        reason = "unknown symbol " + symbol;
    else if (side != '1' && side != '2')
        reason = "invalid side";
    else if (!(qty > 0) || !std::isfinite(qty) || qty > 1e7)
        reason = "invalid quantity";
    else if (!(px > 0) || !std::isfinite(px))
        reason = "invalid price";
    else if (px < q.refPx * 0.9 - 1e-9 || px > q.refPx * 1.1 + 1e-9)
        reason = "price outside +/-10% limit";
    else if (std::fabs(MarketSimulator::RoundToTick(px) - px) > 1e-6)
        reason = "price not on tick (tick=" + std::to_string(MarketSimulator::TickSize(px)).substr(0, 4) + ")";
    else if (m_clToOrder.count(clOrdID))
        reason = "duplicate ClOrdID";

    if (!reason.empty()) {
        o.status = '8';
        ExecEvent e;
        e.execType = '8';
        e.order = o;
        e.clOrdID = clOrdID;
        e.text = reason;
        out.push_back(e);
        return out;
    }

    o.status = '0';
    m_orders[o.orderID] = o;
    m_clToOrder[clOrdID] = o.orderID;
    SimOrder& stored = m_orders[o.orderID];

    ExecEvent ack;
    ack.execType = '0';
    ack.order = stored;
    ack.clOrdID = clOrdID;
    ack.text = "accepted";
    out.push_back(ack);

    TryFill(stored, clOrdID, out);
    return out;
}

std::vector<ExecEvent> MatchingEngine::Cancel(const std::string& clOrdID, const std::string& origClOrdID)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<ExecEvent> out;
    SimOrder* o = Find(origClOrdID);
    if (!o) {
        out.push_back(CancelReject(clOrdID, origClOrdID, '1', "unknown order"));
        return out;
    }
    if (o->status != '0') {
        out.push_back(CancelReject(clOrdID, origClOrdID, '1', "order already " + std::string(o->status == '2' ? "filled" : "closed")));
        return out;
    }
    o->status = '4';
    o->clOrdID = clOrdID;
    m_clToOrder[clOrdID] = o->orderID;

    ExecEvent e;
    e.execType = '4';
    e.order = *o;
    e.clOrdID = clOrdID;
    e.origClOrdID = origClOrdID;
    e.text = "canceled";
    out.push_back(e);
    return out;
}

std::vector<ExecEvent> MatchingEngine::Replace(const std::string& clOrdID, const std::string& origClOrdID,
                                               double qty, double px)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<ExecEvent> out;
    SimOrder* o = Find(origClOrdID);
    if (!o) {
        out.push_back(CancelReject(clOrdID, origClOrdID, '2', "unknown order"));
        return out;
    }
    if (o->status != '0') {
        out.push_back(CancelReject(clOrdID, origClOrdID, '2', "order already " + std::string(o->status == '2' ? "filled" : "closed")));
        return out;
    }
    Quote q;
    m_market.Get(o->symbol, q);
    // 0 keeps the current value (TWSE: 不改量/不改價填 0)
    double newQty = qty > 0 ? qty : o->qty;
    double newPx = px > 0 ? px : o->px;
    if (!std::isfinite(newQty) || newQty < o->cumQty || newQty > 1e7 ||
        !std::isfinite(newPx) || newPx < q.refPx * 0.9 - 1e-9 || newPx > q.refPx * 1.1 + 1e-9 ||
        std::fabs(MarketSimulator::RoundToTick(newPx) - newPx) > 1e-6) {
        out.push_back(CancelReject(clOrdID, origClOrdID, '2', "invalid quantity or price"));
        return out;
    }
    o->qty = newQty;
    o->px = newPx;
    o->clOrdID = clOrdID;
    m_clToOrder[clOrdID] = o->orderID;

    ExecEvent e;
    e.execType = '5';
    e.order = *o;
    e.clOrdID = clOrdID;
    e.origClOrdID = origClOrdID;
    e.text = "replaced";
    out.push_back(e);

    TryFill(*o, clOrdID, out);
    return out;
}

std::vector<ExecEvent> MatchingEngine::Status(const std::string& clOrdID)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<ExecEvent> out;
    SimOrder* o = Find(clOrdID);
    if (!o) {
        out.push_back(CancelReject(clOrdID, clOrdID, '1', "unknown order"));
        return out;
    }
    ExecEvent e;
    e.execType = 'I';
    e.order = *o;
    e.clOrdID = clOrdID;
    e.text = "status";
    out.push_back(e);
    return out;
}

std::vector<ExecEvent> MatchingEngine::OnTick()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<ExecEvent> out;
    for (auto& kv : m_orders)
        TryFill(kv.second, kv.second.clOrdID, out);
    return out;
}

size_t MatchingEngine::OpenOrders() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    size_t n = 0;
    for (const auto& kv : m_orders)
        if (kv.second.status == '0')
            n++;
    return n;
}

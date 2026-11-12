#include "TaifexOrderClient.h"

#include <cmath>
#include <sstream>

using ui::Lv;

static const char* StateName(State s)
{
    switch (s) {
        case State::Logging:       return "Logging";
        case State::Ordering:      return "Ordering";
        case State::SendingLogout: return "SendingLogout";
        case State::Idle:          return "Idle";
    }
    return "?";
}

static double ToNum(const std::string& s)
{
    try {
        size_t used = 0;
        double v = std::stod(s, &used);
        return used == s.size() ? v : NAN;
    } catch (...) {
        return NAN;
    }
}

int Session::RecvSinglePacket(std::string& _outputPacket, unsigned int _flags) {
    _outputPacket.clear();

    while (true) {
        // 先檢查內部 buffer 是否已經有完整封包
        size_t pos = FindCompletePacket();
        if (pos != MAX_SIZE) {
            // 取出完整封包
            _outputPacket.assign(m_recv_buffer.begin(), m_recv_buffer.begin() + pos);
            // 移除已處理的資料
            m_recv_buffer.erase(m_recv_buffer.begin(), m_recv_buffer.begin() + pos);
            ui::Log(Lv::Debug, "recv packet " + std::to_string(pos) + " bytes, " +
                               std::to_string(m_recv_buffer.size()) + " bytes left in buffer");
            return _outputPacket.size();
        }

        // buffer 不夠完整，從 socket 接收新資料
        ssize_t recvSize = recv(m_sockfd, reinterpret_cast<void*>(&m_tmp_recv_buffer[0]), MAX_SIZE, _flags);
        if (recvSize <= 0) {
            return recvSize; // 0: closed, -1: error
        }

        // 加入內部 buffer
        m_recv_buffer.insert(m_recv_buffer.end(),
                            m_tmp_recv_buffer.begin(),
                            m_tmp_recv_buffer.begin() + recvSize);
    }
}

int Session::SendPacket(const std::string &buf)
{
    int sentSize = send(m_sockfd, buf.data(), buf.size(), MSG_NOSIGNAL);
    m_msg_seq_num++;
    return sentSize;
}

std::string Session::SendMsg(const std::function<std::string(uint16_t)>& _build, bool _quiet)
{
    std::lock_guard<std::mutex> lock(m_send_mutex);
    std::string pkt = _build(m_msg_seq_num);
    if (!_quiet)
        ui::LogPacket(Lv::Tx, pkt);
    return SendPacket(pkt) > 0 ? pkt : std::string();
}

/************************************ TaifexOrderClient ************************************/

void TaifexOrderClient::SetState(State _next)
{
    State prev = current_state.exchange(_next);
    if (prev != _next)
        ui::Log(Lv::Session, std::string(StateName(prev)) + " -> " + StateName(_next));
}

std::string TaifexOrderClient::NextClOrdID()
{
    char id[16];
    std::snprintf(id, sizeof(id), "C%06d", m_next_cl_ord_id++);
    return id;
}

/* ------------------------------ decoded messages ------------------------------ */

void TaifexOrderClient::OnExecutionReport(const std::string& _buf)
{
    FIX_8_t r{};
    m_messenger.Receive8(_buf, r);

    std::string root;
    OrderView v;
    {
        std::lock_guard<std::mutex> lock(m_view_mutex);
        if (m_id_to_root.count(r.ClOrdID)) root = m_id_to_root[r.ClOrdID];
        else if (m_id_to_root.count(r.OrigClOrdID)) root = m_id_to_root[r.OrigClOrdID];
        else root = r.ClOrdID;
        OrderView& ov = m_orders[root];
        ov.root = root;
        ov.orderID = r.OrderID;
        ov.symbol = r.Symbol;
        ov.side = r.Side;
        ov.qty = r.OrderQty;
        ov.px = r.Price;
        ov.cumQty = r.CumQty;
        ov.avgPx = r.AvgPx;
        ov.status = ui::OrdStatusName(r.OrdStatus);
        ov.text = r.Text;
        if (r.ExecType != '8' && r.ExecType != 'I')
            ov.latestID = r.ClOrdID;
        v = ov;
    }

    std::ostringstream s;
    std::string what = ui::SideName(v.side) + " " + v.symbol + " " + ui::Fmt(v.qty, 0) + " @ " + ui::Fmt(v.px);
    switch (r.ExecType) {
        case '0':
            s << "NEW      " << root << "  " << what << "  (OrderID " << v.orderID << ", resting)";
            ui::Log(Lv::Order, s.str());
            break;
        case 'F':
            s << "FILLED   " << root << "  " << ui::SideName(v.side) << " " << v.symbol << " " << ui::Fmt(r.LastQty, 0)
              << " @ " << ui::Fmt(r.LastPx) << "  cum " << ui::Fmt(v.cumQty, 0) << "/" << ui::Fmt(v.qty, 0)
              << "  avg " << ui::Fmt(v.avgPx);
            ui::Log(Lv::Fill, s.str());
            break;
        case '4':
            s << "CANCELED " << root << "  " << what << "  (filled " << ui::Fmt(v.cumQty, 0) << ")";
            ui::Log(Lv::Order, s.str());
            break;
        case '5':
            s << "REPLACED " << root << "  now " << what;
            ui::Log(Lv::Order, s.str());
            break;
        case '8':
            s << "REJECTED " << root << "  " << what << "  reason: " << r.Text;
            ui::Log(Lv::Reject, s.str());
            break;
        case 'I':
            s << "STATUS   " << root << "  " << what << "  " << v.status << "  cum " << ui::Fmt(v.cumQty, 0)
              << "/" << ui::Fmt(v.qty, 0);
            ui::Log(Lv::Order, s.str());
            break;
        default:
            s << "ExecType " << r.ExecType << " for " << root;
            ui::Log(Lv::Order, s.str());
            break;
    }
}

void TaifexOrderClient::OnCancelReject(const std::string& _buf)
{
    FIX_9_t r{};
    m_messenger.Receive9(_buf, r);
    std::string root;
    {
        std::lock_guard<std::mutex> lock(m_view_mutex);
        if (m_id_to_root.count(r.OrigClOrdID)) root = m_id_to_root[r.OrigClOrdID];
        else if (m_id_to_root.count(r.ClOrdID)) root = m_id_to_root[r.ClOrdID];
        else root = r.OrigClOrdID;
        auto it = m_orders.find(root);
        if (it != m_orders.end())
            it->second.text = r.Text;
    }
    std::string req = r.CxlRejResponseTo == '2' ? "replace" : r.CxlRejResponseTo == '1' ? "cancel/status" : "request";
    ui::Log(Lv::Reject, req + " rejected for " + root + ": " + r.Text);
}

void TaifexOrderClient::OnMarketData(const std::string& _buf)
{
    MDQuote q;
    if (!m_messenger.ReceiveW(_buf, q))
        return;
    bool watch;
    {
        std::lock_guard<std::mutex> lock(m_view_mutex);
        m_quotes[q.Symbol] = q;
        watch = m_watch;
    }
    if (watch) {
        std::ostringstream s;
        s << std::left << std::setw(6) << q.Symbol << " bid " << ui::Fmt(q.Bid) << " x " << ui::Fmt(q.BidSize, 0)
          << " | ask " << ui::Fmt(q.Ask) << " x " << ui::Fmt(q.AskSize, 0) << " | last " << ui::Fmt(q.Last);
        ui::Log(Lv::Market, s.str());
    }
}

/* ------------------------------ command line ------------------------------ */

void TaifexOrderClient::PrintHelp()
{
    using namespace ui;
    Block({C(ansi::Bold) + "  Commands" + C(ansi::Reset) + "  (qty in shares, 1 lot = 1000)",
           "    buy  <symbol> <qty> <price>     New Order Single           (35=D)",
           "    sell <symbol> <qty> <price>     New Order Single           (35=D)",
           "    cancel  <ClOrdID>               Order Cancel Request       (35=F)",
           "    replace <ClOrdID> <qty> <price> Order Cancel/Replace       (35=G, 0 = keep)",
           "    status  <ClOrdID>               Order Status Request       (35=H)",
           "    quotes                          latest market data (from 35=W)",
           "    orders                          orders rebuilt from 35=8 / 35=9 reports",
           "    watch on|off                    print every quote update",
           "    raw on|off                      show packets as FIX tag tables",
           "    verbose on|off                  debug output",
           "    hb | test | logout              Heartbeat(0) / TestRequest(1) / Logout(5)",
           "    wait <sec>                      pause (for scripted input)",
           "    help"});
}

void TaifexOrderClient::PrintQuotes()
{
    using namespace ui;
    std::vector<std::string> lines;
    std::lock_guard<std::mutex> lock(m_view_mutex);
    if (m_quotes.empty()) {
        Block({"  no market data received yet"});
        return;
    }
    std::ostringstream h;
    h << C(ansi::Dim) << "  " << std::left << std::setw(8) << "SYMBOL" << std::right << std::setw(10) << "BID"
      << std::setw(9) << "BID SZ" << std::setw(10) << "ASK" << std::setw(9) << "ASK SZ" << std::setw(10) << "LAST"
      << std::setw(11) << "VOLUME" << C(ansi::Reset);
    lines.push_back(h.str());
    for (const auto& kv : m_quotes) {
        const MDQuote& q = kv.second;
        std::ostringstream r;
        r << "  " << std::left << std::setw(8) << q.Symbol << std::right << std::setw(10) << Fmt(q.Bid)
          << std::setw(9) << Fmt(q.BidSize, 0) << std::setw(10) << Fmt(q.Ask) << std::setw(9) << Fmt(q.AskSize, 0)
          << std::setw(10) << Fmt(q.Last) << std::setw(11) << Fmt(q.Volume, 0);
        lines.push_back(r.str());
    }
    Block(lines);
}

void TaifexOrderClient::PrintOrders()
{
    using namespace ui;
    std::vector<std::string> lines;
    std::lock_guard<std::mutex> lock(m_view_mutex);
    if (m_orders.empty()) {
        Block({"  no orders yet"});
        return;
    }
    std::ostringstream h;
    h << C(ansi::Dim) << "  " << std::left << std::setw(9) << "CLORDID" << std::setw(9) << "ORDERID" << std::setw(5)
      << "SIDE" << std::setw(7) << "SYMBOL" << std::right << std::setw(8) << "QTY" << std::setw(10) << "PRICE"
      << std::setw(8) << "CUM" << std::setw(10) << "AVGPX" << "  " << std::left << std::setw(9) << "STATUS"
      << "TEXT" << C(ansi::Reset);
    lines.push_back(h.str());
    for (const auto& kv : m_orders) {
        const OrderView& o = kv.second;
        const char* color = o.status == "FILLED" ? ansi::Green
                          : o.status == "REJECTED" || o.status == "CANCELED" ? ansi::Red
                          : ansi::Yellow;
        std::ostringstream r;
        r << "  " << std::left << std::setw(9) << o.root << std::setw(9) << o.orderID << std::setw(5)
          << SideName(o.side) << std::setw(7) << o.symbol << std::right << std::setw(8) << Fmt(o.qty, 0)
          << std::setw(10) << Fmt(o.px) << std::setw(8) << Fmt(o.cumQty, 0) << std::setw(10) << Fmt(o.avgPx)
          << "  " << C(color) << std::left << std::setw(9) << o.status << C(ansi::Reset) << o.text;
        lines.push_back(r.str());
    }
    Block(lines);
}

// Show what the "Order Out Register" holds, encode it and send it.
void TaifexOrderClient::SendOrder(std::shared_ptr<Session> _session, char _type, OrderParams _p, const std::string& _root)
{
    using namespace ui;
    const std::string type(1, _type);
    std::ostringstream reg;
    reg << "ClOrdID=" << _p.ClOrdID;
    if (!_p.OrigClOrdID.empty()) reg << " OrigClOrdID=" << _p.OrigClOrdID;
    if (!_p.Symbol.empty()) reg << " Symbol=" << _p.Symbol;
    if (_type == 'D' || _type == 'G') reg << " Side=" << SideName(_p.Side) << " Qty=" << _p.Qty << " Price=" << _p.Price;
    Block({C(ansi::Cyan) + "  +-- Order Out Register -> FIX encoder: " + MsgTypeName(type) + " (35=" + type + ")" + C(ansi::Reset),
           C(ansi::Cyan) + "  |   " + C(ansi::Reset) + reg.str()});

    {
        std::lock_guard<std::mutex> lock(m_view_mutex);
        m_id_to_root[_p.ClOrdID] = _root;
    }

    std::string pkt = _session->SendMsg([&](uint16_t seq) {
        const FIXhdr_t hdr = Hdr();
        switch (_type) {
            case 'D': return m_messenger.MakeD(_p, hdr, seq);
            case 'F': return m_messenger.MakeF(_p, hdr, seq);
            case 'G': return m_messenger.MakeG(_p, hdr, seq);
            default:  return m_messenger.MakeH(_p, hdr, seq);
        }
    });
    if (pkt.empty())
        Log(Lv::Warn, "send failed");
    else if (!Cfg().raw)
        Block({C(ansi::Cyan) + "  +-> wire: " + C(ansi::Reset) + C(ansi::Dim) + OneLineFIX(pkt) + C(ansi::Reset)});
}

bool TaifexOrderClient::Execute(std::shared_ptr<Session> _session, const std::string& _line)
{
    std::istringstream in(_line);
    std::vector<std::string> a;
    for (std::string w; in >> w;)
        a.push_back(w);
    if (a.empty())
        return true;
    std::string cmd = a[0];
    for (auto& c : cmd) c = std::tolower(c);
    auto onOff = [&](bool& flag, const char* name) {
        if (a.size() > 1) flag = (a[1] == "on" || a[1] == "1");
        else flag = !flag;
        ui::Log(Lv::Info, std::string(name) + (flag ? " on" : " off"));
    };
    // Resolve any ClOrdID the user typed to its order view (if known).
    auto lookup = [&](const std::string& id, OrderView& out) {
        std::lock_guard<std::mutex> lock(m_view_mutex);
        std::string root = m_id_to_root.count(id) ? m_id_to_root[id] : id;
        auto it = m_orders.find(root);
        if (it == m_orders.end()) {
            out = OrderView();
            out.root = id;
            out.latestID = id;
            return false;
        }
        out = it->second;
        return true;
    };

    if (cmd == "help" || cmd == "?") {
        PrintHelp();
    }
    else if (cmd == "buy" || cmd == "sell") {
        if (a.size() != 4) { ui::Log(Lv::Warn, "usage: " + cmd + " <symbol> <qty> <price>"); return true; }
        OrderParams p;
        p.Symbol = a[1];
        p.Side = cmd == "buy" ? '1' : '2';
        p.Qty = ToNum(a[2]);
        p.Price = ToNum(a[3]);
        if (!(p.Qty > 0) || p.Qty >= 1e6 || std::floor(p.Qty) != p.Qty || !(p.Price > 0) || p.Price >= 1e5) {
            ui::Log(Lv::Warn, "qty must be a whole number in (0, 999999], price must be in (0, 99999]");
            return true;
        }
        p.ClOrdID = NextClOrdID();
        {
            std::lock_guard<std::mutex> lock(m_view_mutex);
            OrderView& v = m_orders[p.ClOrdID];
            v.root = v.latestID = p.ClOrdID;
            v.symbol = p.Symbol;
            v.side = p.Side;
            v.qty = p.Qty;
            v.px = p.Price;
        }
        SendOrder(_session, 'D', p, p.ClOrdID);
    }
    else if (cmd == "cancel") {
        if (a.size() != 2) { ui::Log(Lv::Warn, "usage: cancel <ClOrdID>"); return true; }
        OrderView v;
        if (!lookup(a[1], v)) ui::Log(Lv::Info, a[1] + " is not a known order, sending anyway");
        OrderParams p;
        p.ClOrdID = NextClOrdID();
        p.OrigClOrdID = v.latestID;
        p.Symbol = v.symbol;
        p.Side = v.side;
        SendOrder(_session, 'F', p, v.root);
    }
    else if (cmd == "replace") {
        if (a.size() != 4) { ui::Log(Lv::Warn, "usage: replace <ClOrdID> <qty> <price>   (0 keeps the value)"); return true; }
        OrderView v;
        if (!lookup(a[1], v)) ui::Log(Lv::Info, a[1] + " is not a known order, sending anyway");
        OrderParams p;
        p.ClOrdID = NextClOrdID();
        p.OrigClOrdID = v.latestID;
        p.Symbol = v.symbol;
        p.Side = v.side;
        p.Qty = ToNum(a[2]);
        p.Price = ToNum(a[3]);
        if (!(p.Qty >= 0) || p.Qty >= 1e6 || !(p.Price >= 0) || p.Price >= 1e5) {
            ui::Log(Lv::Warn, "invalid qty or price");
            return true;
        }
        SendOrder(_session, 'G', p, v.root);
    }
    else if (cmd == "status") {
        if (a.size() != 2) { ui::Log(Lv::Warn, "usage: status <ClOrdID>"); return true; }
        OrderView v;
        lookup(a[1], v);
        OrderParams p;
        p.ClOrdID = v.latestID;
        p.Symbol = v.symbol;
        p.Side = v.side;
        SendOrder(_session, 'H', p, v.root);
    }
    else if (cmd == "quotes" || cmd == "q") {
        PrintQuotes();
    }
    else if (cmd == "orders" || cmd == "o") {
        PrintOrders();
    }
    else if (cmd == "watch") {
        std::lock_guard<std::mutex> lock(m_view_mutex);
        onOff(m_watch, "watch");
    }
    else if (cmd == "raw") {
        onOff(ui::Cfg().raw, "raw");
    }
    else if (cmd == "verbose") {
        onOff(ui::Cfg().verbose, "verbose");
    }
    else if (cmd == "hb" || cmd == "heartbeat") {
        _session->SendMsg([&](uint16_t seq) { return m_messenger.Make0("", Hdr(), seq); });
    }
    else if (cmd == "test" || cmd == "testrequest") {
        _session->SendMsg([&](uint16_t seq) { return m_messenger.Make1("", Hdr(), seq); });
    }
    else if (cmd == "logout" || cmd == "quit" || cmd == "exit") {
        {
            std::lock_guard<std::mutex> lock(m_life_mutex);
            m_logout_start = std::chrono::steady_clock::now();
        }
        SetState(State::SendingLogout);
        _session->SendMsg([&](uint16_t seq) { return m_messenger.Make5("", Hdr(), seq); });
        return false;
    }
    else if (cmd == "wait" || cmd == "sleep") {
        double sec = a.size() > 1 ? ToNum(a[1]) : 1;
        if (sec > 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(sec * 1000)));
    }
    else if (cmd == "neworder" || cmd == "ordercancelrequest" || cmd == "ordercancel" || cmd == "orderstatus") {
        ui::Log(Lv::Info, "'" + a[0] + "' now takes parameters: use buy/sell, cancel, replace or status (type help)");
    }
    else {
        ui::Log(Lv::Warn, "unknown command '" + a[0] + "', type help");
    }
    return true;
}

void TaifexOrderClient::CommandLoop(std::shared_ptr<Session> _session)
{
    {
        std::unique_lock<std::mutex> lock(m_life_mutex);
        m_life_cv.wait(lock, [this] { return is_logon || !m_running; });
        if (!m_running)
            return;
    }
    const bool interactive = isatty(STDIN_FILENO);
    PrintHelp();
    if (interactive)
        ui::ShowPrompt();

    std::string line;
    while (true) {
        if (!std::getline(std::cin, line)) {
            ui::HidePrompt();
            std::lock_guard<std::mutex> lock(m_life_mutex);
            if (!m_running || current_state != State::Ordering)
                return;
            ui::Log(Lv::Info, "end of input, logging out");
            m_logout_start = std::chrono::steady_clock::now();
            SetState(State::SendingLogout);
            _session->SendMsg([&](uint16_t seq) { return m_messenger.Make5("", Hdr(), seq); });
            return;
        }
        ui::HidePrompt();
        {
            std::lock_guard<std::mutex> lock(m_life_mutex);
            if (!m_running)
                return;
        }
        if (!interactive && !line.empty())
            ui::Log(Lv::Info, "> " + line);
        if (!Execute(_session, line))
            return;
        if (interactive)
            ui::ShowPrompt();
    }
}

/* ------------------------------ session ------------------------------ */

int TaifexOrderClient::Start()
{
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd < 0) {
        ui::Log(Lv::Warn, "Socket creation failed");
        return 1;
    }

    sockaddr_in clientAdder{};
    clientAdder.sin_family = AF_INET;
    clientAdder.sin_port = htons(m_port);
    // Convert m_client_ip into binary form
    if (inet_pton(AF_INET, m_client_ip.c_str(), &clientAdder.sin_addr) <= 0) {
        ui::Log(Lv::Warn, "Invalid address: " + m_client_ip);
        return 1;
    }
    {
        using namespace ui;
        Block({C(ansi::Bold) + "==============================================================" + C(ansi::Reset),
               C(ansi::Bold) + "  HFST Pseudo Client (FPGA protocol front-end model)" + C(ansi::Reset),
               "  role   : FIX 4.4 encode/decode only; you are the trading algorithm",
               "  server : " + m_client_ip + ":" + std::to_string(m_port),
               "  output : " + std::string(Cfg().raw ? "raw FIX tables" : "one-line packets") +
                   (Cfg().verbose ? ", verbose" : ""),
               C(ansi::Bold) + "==============================================================" + C(ansi::Reset)});
    }
    // Connect to server
    if (connect(m_sockfd, (struct sockaddr*)&clientAdder, sizeof(clientAdder)) < 0) {
        ui::Log(Lv::Warn, std::string("Connection failed: ") + strerror(errno));
        return 1;
    }
    ui::Log(Lv::Session, "TCP connected, sending Logon");

    // 將連線封裝成 Session 物件
    auto session = std::make_shared<Session>(m_sockfd);
    m_sockfd = -1;  // owned by the session from now on
    // start login
    std::string buffer;
    int recvSize = 0;
    int recvMsgError = 0;
    m_hdr.MsgSeqNum = 0;
    SetState(State::Logging);
    current_order_state = OrderState::Idle;
    std::thread delayedThread([this, session]()
    {
        while(1)
        {
            EnableHeartBeat();
            sleep(m_heartbeat_interval_sec * 2);
            if (m_en_hb) {
                session->SendMsg([&](uint16_t seq) { return m_messenger.Make0("", Hdr(), seq); });
                m_sent_0_cnt++;
            }
        }
    });
    delayedThread.detach();

    SetHeartBeatIntervalInSec(10);
    session->SendMsg([&](uint16_t seq) { return m_messenger.MakeA(buffer, Hdr(), seq); });

    std::thread commandThread(&TaifexOrderClient::CommandLoop, this, session);
    commandThread.detach();

    auto send = [&](const std::function<std::string(uint16_t)>& build) { session->SendMsg(build); };
    bool done = false;
    while (!done) {
        recvSize = session->RecvSinglePacket(buffer);
        m_rcv = false;
        if (recvSize <= 0)
        {
            ui::Log(Lv::Warn, "connection closed by server");
            break;
        }
        const bool isMarketData = ui::FIXField(buffer, "35") == "W";
        if (!isMarketData || ui::Cfg().verbose)
            ui::LogPacket(Lv::Rx, buffer);
        {
            std::lock_guard<std::mutex> lock(m_hdr_mutex);
            recvMsgError = m_messenger.SearchMsgType(buffer, m_hdr, m_2_data, m_3_data);
        }
        if (recvMsgError)
            ui::Log(Lv::Debug, "SearchMsgType returned " + std::to_string(recvMsgError));
        const FIXhdr_t hdr = Hdr();

        switch (current_state.load()) {
            case State::Logging:
                if (hdr.MessageType == order.FIXMsgType["Logon"] && !recvMsgError) {
                    m_rcv = true;
                    ui::Log(Lv::Session, "Logon accepted by exchange");
                    SetState(State::Ordering);
                    {
                        std::lock_guard<std::mutex> lock(m_life_mutex);
                        is_logon = true;
                    }
                    m_life_cv.notify_all();
                }
                else if (hdr.MessageType == order.FIXMsgType["ResendRequest"] && !recvMsgError) {
                    m_rcv = true;
                    SetHeartBeatIntervalInSec(10);
                    send([&](uint16_t seq) { return m_messenger.MakeA(buffer, hdr, seq); });
                }
                else if (hdr.MessageType == order.FIXMsgType["RejectSession"] && !recvMsgError) {
                    m_rcv = true;
                    send([&](uint16_t seq) { return m_messenger.Make3resp(buffer, hdr, seq); });
                }
                else if (hdr.MessageType == order.FIXMsgType["Logout"] || hdr.MessageType == order.FIXMsgType["Heartbeat"] || hdr.MessageType == order.FIXMsgType["TestRequest"]) {
                    m_rcv = true;
                    send([&](uint16_t seq) { return m_messenger.Make5(buffer, hdr, seq); });
                    SetState(State::Idle);
                    done = true;
                }
                else if (recvMsgError == 2) {
                    m_rcv = true;
                    send([&](uint16_t seq) { return m_messenger.Make2(&m_2_data, hdr, seq); });
                }
                else if (recvMsgError == 3) {
                    m_rcv = true;
                    send([&](uint16_t seq) { return m_messenger.Make3(&m_3_data, hdr, seq); });
                }
                else if (recvMsgError == 5) {
                    m_rcv = true;
                    send([&](uint16_t seq) { return m_messenger.Make5(buffer, hdr, seq); });
                    {
                        std::lock_guard<std::mutex> lock(m_life_mutex);
                        m_logout_start = std::chrono::steady_clock::now();
                    }
                    SetState(State::SendingLogout);
                }
                else {
                    SetHeartBeatIntervalInSec(10);
                    send([&](uint16_t seq) { return m_messenger.MakeA(buffer, hdr, seq); });
                }
                break;
            case State::Ordering:
                if (hdr.MessageType == order.FIXMsgType["ExecutionReport"]) {
                    m_rcv = true;
                    OnExecutionReport(buffer);
                }
                else if (hdr.MessageType == order.FIXMsgType["OrderCancelReject"]) {
                    m_rcv = true;
                    OnCancelReject(buffer);
                }
                else if (hdr.MessageType == order.FIXMsgType["MarketData"]) {
                    m_rcv = true;
                    OnMarketData(buffer);
                }
                else if (hdr.MessageType == order.FIXMsgType["TestRequest"] && !recvMsgError) {
                    m_rcv = true;
                    send([&](uint16_t seq) { return m_messenger.Make0(buffer, hdr, seq); });
                }
                else if (hdr.MessageType == order.FIXMsgType["ResendRequest"] && !recvMsgError) {
                    m_rcv = true;
                    send([&](uint16_t seq) { return m_messenger.Make2resp(buffer, hdr, seq); });
                }
                else if (hdr.MessageType == order.FIXMsgType["RejectSession"] && !recvMsgError) {
                    m_rcv = true;
                    send([&](uint16_t seq) { return m_messenger.Make3resp(buffer, hdr, seq); });
                }
                else if (hdr.MessageType == order.FIXMsgType["Logout"]) {
                    m_rcv = true;
                    send([&](uint16_t seq) { return m_messenger.Make5(buffer, hdr, seq); });
                    ui::Log(Lv::Session, "exchange logged us out");
                    SetState(State::Idle);
                    done = true;
                }
                else if (recvMsgError == 2) {
                    m_rcv = true;
                    send([&](uint16_t seq) { return m_messenger.Make2(&m_2_data, hdr, seq); });
                }
                else if (recvMsgError == 3) {
                    m_rcv = true;
                    send([&](uint16_t seq) { return m_messenger.Make3(&m_3_data, hdr, seq); });
                }
                else if (hdr.MessageType == order.FIXMsgType["Heartbeat"]) {
                    m_rcv = true;
                }
                else {
                    ui::Log(Lv::Warn, "unhandled message 35=" + hdr.MessageType + " (SearchMsgType=" +
                                          std::to_string(recvMsgError) + ")");
                }
                break;
            case State::SendingLogout:
                if (hdr.MessageType == order.FIXMsgType["Logout"]) {
                    m_rcv = true;
                    ui::Log(Lv::Session, "Logout confirmed by exchange");
                    SetState(State::Idle);
                    done = true;
                }
                else if ((hdr.MessageType == order.FIXMsgType["Heartbeat"] && !recvMsgError) || recvMsgError == 3 || recvMsgError == 2) {
                    m_rcv = true;
                }
                else if (hdr.MessageType == order.FIXMsgType["TestRequest"] && !recvMsgError) {
                    m_rcv = true;
                    send([&](uint16_t seq) { return m_messenger.Make0(buffer, hdr, seq); });
                }
                else {
                    // reports still in flight are displayed, but nothing is sent
                    if (hdr.MessageType == order.FIXMsgType["ExecutionReport"]) OnExecutionReport(buffer);
                    else if (hdr.MessageType == order.FIXMsgType["OrderCancelReject"]) OnCancelReject(buffer);
                    else if (hdr.MessageType == order.FIXMsgType["MarketData"]) OnMarketData(buffer);
                    std::chrono::steady_clock::time_point start;
                    {
                        std::lock_guard<std::mutex> lock(m_life_mutex);
                        start = m_logout_start;
                    }
                    std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - start;
                    if (elapsed.count() > 5) {
                        ui::Log(Lv::Warn, "no Logout reply within 5 s, closing");
                        SetState(State::Idle);
                        done = true;
                    }
                }
                break;
            default:
                m_rcv = false;
                break;
        }
    }

    {
        std::lock_guard<std::mutex> lock(m_life_mutex);
        m_running = false;
    }
    m_life_cv.notify_all();
    ui::HidePrompt();
    PrintOrders();
    ui::Log(Lv::Session, "session closed");
    return 0;
}

static void PrintUsage()
{
    std::cout << "Usage: TaifexOrderClient [--host IP] [--port N] [--raw] [--verbose] [--no-color]\n"
                 "  --host IP    server address (default 127.0.0.1)\n"
                 "  --port N     server port (default 9000)\n"
                 "  --raw        print every FIX packet as a tag table\n"
                 "  --verbose    debug output, including every MarketData (W) packet\n"
                 "  --no-color   disable ANSI colours\n";
}

int main(int argc, char* argv[])
{
    std::string host = "127.0.0.1";
    int port = 9000;
    for (int i = 1; i < argc; i++) {
        std::string a = argv[i];
        if ((a == "--host" || a == "--port") && i + 1 >= argc) { PrintUsage(); return 1; }
        if (a == "--host") host = argv[++i];
        else if (a == "--port") port = std::atoi(argv[++i]);
        else if (a == "--raw") ui::Cfg().raw = true;
        else if (a == "--verbose") ui::Cfg().verbose = true;
        else if (a == "--no-color") ui::Cfg().color = false;
        else { PrintUsage(); return a == "--help" || a == "-h" ? 0 : 1; }
    }
    TaifexOrderClient client(host, port);
    return client.Start();
}

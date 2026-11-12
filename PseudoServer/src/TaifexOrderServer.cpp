#include "TaifexOrderServer.h"

#include <cmath>

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

int Session::RecvSinglePacket(std::string& outputPacket, unsigned int flags) {
    outputPacket.clear();

    while (true) {
        // 先檢查內部 buffer 是否已經有完整封包
        size_t pos = findCompletePacket();
        if (pos != MAX_SIZE) {
            // 取出完整封包
            outputPacket.assign(m_recvBuffer.begin(), m_recvBuffer.begin() + pos);
            // 移除已處理的資料
            m_recvBuffer.erase(m_recvBuffer.begin(), m_recvBuffer.begin() + pos);
            ui::Log(Lv::Debug, "recv packet " + std::to_string(pos) + " bytes, " +
                               std::to_string(m_recvBuffer.size()) + " bytes left in buffer");
            return outputPacket.size();
        }

        // buffer 不夠完整，從 socket 接收新資料
        ssize_t recvSize = recv(m_sockfd, reinterpret_cast<void*>(&m_tmpRecvBuffer[0]), MAX_SIZE, flags);
        if (recvSize <= 0) {
            return recvSize; // 0: closed, -1: error
        }

        // 加入內部 buffer
        m_recvBuffer.insert(m_recvBuffer.end(),
                            m_tmpRecvBuffer.begin(),
                            m_tmpRecvBuffer.begin() + recvSize);
    }
}

int Session::SendPacket(const std::string &buf)
{
    int sentSize = send(m_sockfd, buf.data(), buf.size(), MSG_NOSIGNAL);
    m_msgSeqNum++;
    return sentSize;
}

int Session::SendMsg(const std::function<std::string(uint16_t)>& build, bool quiet)
{
    std::lock_guard<std::mutex> lock(m_sendMutex);
    std::string pkt = build(m_msgSeqNum);
    if (!quiet)
        ui::LogPacket(Lv::Tx, pkt);
    return SendPacket(pkt);
}

/************************************ TaifexOrderServer ************************************/

void TaifexOrderServer::SetState(State next)
{
    if (next != currentState)
        ui::Log(Lv::Session, std::string(StateName(currentState)) + " -> " + StateName(next));
    currentState = next;
}

void TaifexOrderServer::PrintBoard()
{
    using namespace ui;
    std::vector<std::string> lines;
    std::ostringstream head;
    head << C(ansi::Bold) << "  ---- MARKET " << Now() << "  open orders: " << m_engine.OpenOrders()
         << " ----" << C(ansi::Reset);
    lines.push_back(head.str());
    std::ostringstream cols;
    cols << C(ansi::Dim) << "  " << std::left << std::setw(8) << "SYMBOL" << std::right << std::setw(10) << "BID"
         << std::setw(10) << "ASK" << std::setw(10) << "LAST" << std::setw(9) << "CHG%" << std::setw(11) << "VOLUME"
         << C(ansi::Reset);
    lines.push_back(cols.str());
    for (const auto& q : m_market.Snapshot()) {
        double chg = (q.last - q.refPx) / q.refPx * 100.0;
        // Taiwan convention: red = up, green = down.
        const char* color = chg > 0 ? ansi::Red : chg < 0 ? ansi::Green : ansi::White;
        std::ostringstream row;
        row << "  " << std::left << std::setw(8) << q.symbol << std::right << std::setw(10) << Fmt(q.bid)
            << std::setw(10) << Fmt(q.ask) << C(color) << std::setw(10) << Fmt(q.last) << std::setw(8)
            << ((chg > 0 ? "+" : "") + Fmt(chg)) << "%" << C(ansi::Reset) << std::setw(11) << q.volume;
        lines.push_back(row.str());
    }
    Block(lines);
}

void TaifexOrderServer::SendExecEvent(const std::shared_ptr<Session>& session, const ExecEvent& e)
{
    const SimOrder& o = e.order;
    std::ostringstream desc;
    desc << e.clOrdID << " " << ui::SideName(o.side) << " " << o.symbol << " " << o.qty << " @ " << ui::Fmt(o.px);

    if (e.isCancelReject) {
        FIX_9_t m;
        m.OrderID = o.orderID;
        m.ClOrdID = e.clOrdID;
        m.OrigClOrdID = e.origClOrdID;
        m.OrdStatus = o.status;
        m.Account = "SIM";
        m.CxlRejResponseTo = e.cxlRejResponseTo;
        // Serialize9 streams this uint8_t as a character: store the ASCII
        // digit. 0 = too late to cancel, 1 = unknown order.
        m.CxlRejReason = o.orderID == "NONE" ? '1' : '0';
        m.Text = e.text;
        session->SendMsg([&](uint16_t seq) { return m_messenger.Make9(m, Hdr(), seq); });
        m_sent_9_cnt++;
        ui::Log(Lv::Reject, "CANCEL REJECT " + e.origClOrdID + " (" + e.text + ")");
        return;
    }

    FIX_8_t m;
    char execID[16];
    std::snprintf(execID, sizeof(execID), "E%06d", ++m_exec_id);
    m.OrderID = o.orderID;
    m.ClOrdID = e.clOrdID;
    m.OrigClOrdID = e.origClOrdID;
    m.ExecID = execID;
    m.ExecType = e.execType;
    m.OrdStatus = o.status;
    m.OrdRejReason = e.execType == '8' ? 99 : 0;
    m.Account = "SIM";
    m.Symbol = o.symbol;
    m.Side = o.side;
    m.OrderQty = o.qty;
    m.OrdType = '2';
    m.TimeInForce = '0';
    m.Price = o.px;
    m.LastQty = e.lastQty;
    m.LastPx = e.lastPx;
    m.LeavesQty = o.status == '0' ? o.qty - o.cumQty : 0;
    m.CumQty = o.cumQty;
    m.AvgPx = o.avgPx;
    m.Text = e.text;
    m.TwseIvacnoFlag = '1';
    m.TwseOrdType = '0';
    m.TwseExCode = '0';
    session->SendMsg([&](uint16_t seq) { return m_messenger.Make8(m, Hdr(), seq); });
    m_sent_8_cnt++;

    switch (e.execType) {
        case 'F':
            ui::Log(Lv::Fill, "FILLED " + e.clOrdID + " " + ui::SideName(o.side) + " " + o.symbol + " " +
                                  ui::Fmt(e.lastQty, 0) + " @ " + ui::Fmt(e.lastPx) + "  (cum " +
                                  ui::Fmt(o.cumQty, 0) + "/" + ui::Fmt(o.qty, 0) + ")");
            break;
        case '8':
            ui::Log(Lv::Reject, "REJECTED " + desc.str() + " (" + e.text + ")");
            break;
        default:
            ui::Log(Lv::Order, ui::ExecTypeName(e.execType) + " " + desc.str() + "  [" + o.orderID + ", " +
                                   ui::OrdStatusName(o.status) + "]");
            break;
    }
}

void TaifexOrderServer::HandleOrder(const std::shared_ptr<Session>& session, const std::string& buffer)
{
    const std::string type = Hdr().MessageType;
    auto field = [&](const char* tag) {
        std::string v = m_messenger.DecodeMessage(buffer, tag);
        return v == "no this tag" ? std::string() : v;
    };
    std::string clOrdID = field("11");
    std::string orig = field("41");
    std::vector<ExecEvent> events;

    if (type == order.FIXMsgType["NewOrder"]) {
        std::string side = field("54");
        events = m_engine.NewOrder(clOrdID, field("55"), side.empty() ? '?' : side[0],
                                   ToNum(field("38")), ToNum(field("44")));
        m_recv_D_cnt++;
    } else if (type == order.FIXMsgType["OrderCancelRequest"]) {
        events = m_engine.Cancel(clOrdID, orig);
    } else if (type == order.FIXMsgType["OrderCancel"]) {  // G: cancel/replace
        events = m_engine.Replace(clOrdID, orig, ToNum(field("38")), ToNum(field("44")));
    } else if (type == order.FIXMsgType["OrderStatus"]) {
        events = m_engine.Status(clOrdID);
    }
    for (const auto& e : events)
        SendExecEvent(session, e);
}

void TaifexOrderServer::MarketLoop(const std::shared_ptr<Session>& session, std::atomic<bool>& running)
{
    auto lastBoard = std::chrono::steady_clock::now() - std::chrono::seconds(m_board_sec);
    int mdReq = 0;
    while (running) {
        for (int waited = 0; waited < m_tick_ms && running; waited += 50)
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        if (!running)
            break;

        m_market.Tick();
        bool quiet = !ui::Cfg().verbose;
        for (const auto& q : m_market.Snapshot()) {
            FIX_W_t w;
            w.MDReqID = "MD" + std::to_string(++mdReq);
            w.Symbol = q.symbol;
            w.BidPx = q.bid;
            w.BidSize = q.bidSz;
            w.OfferPx = q.ask;
            w.OfferSize = q.askSz;
            w.LastPx = q.last;
            w.LastSize = q.volume;
            int sent = session->SendMsg([&](uint16_t seq) { return m_messenger.MakeW(w, Hdr(), seq); }, quiet);
            if (sent <= 0) {
                running = false;
                return;
            }
        }
        for (const auto& e : m_engine.OnTick())
            SendExecEvent(session, e);

        auto now = std::chrono::steady_clock::now();
        if (m_board_sec > 0 && now - lastBoard >= std::chrono::seconds(m_board_sec)) {
            PrintBoard();
            lastBoard = now;
        }
    }
}

int TaifexOrderServer::Start()
{
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(m_port);
    if (bind(m_sockfd, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0)
    {
        ui::Log(Lv::Warn, std::string("Bind failed: ") + strerror(errno));
        std::exit(EXIT_FAILURE);
    }

    if (listen(m_sockfd, m_expectedSessions) < 0)
    {
        ui::Log(Lv::Warn, std::string("Listen failed: ") + strerror(errno));
        std::exit(EXIT_FAILURE);
    }

    {
        using namespace ui;
        std::string symbols;
        for (const auto& q : m_market.Snapshot())
            symbols += q.symbol + " ";
        Block({C(ansi::Bold) + "==============================================================" + C(ansi::Reset),
               C(ansi::Bold) + "  HFST Pseudo Exchange (FIX 4.4)" + C(ansi::Reset),
               "  listening  : 0.0.0.0:" + std::to_string(m_port) + "  (" +
                   std::to_string(m_expectedSessions) + " session(s))",
               "  symbols    : " + symbols,
               "  market tick: " + std::to_string(m_tick_ms) + " ms, board every " + std::to_string(m_board_sec) + " s",
               "  output     : " + std::string(Cfg().raw ? "raw FIX tables" : "one-line packets") +
                   (Cfg().verbose ? ", verbose" : ""),
               C(ansi::Bold) + "==============================================================" + C(ansi::Reset)});
    }

    for (int i = 0; i < m_expectedSessions; i++)
    {
        sockaddr_in clientAddr{};
        socklen_t addrLen = sizeof(clientAddr);
        ui::Log(Lv::Info, "Waiting for connection...");
        int clientSockfd = accept(m_sockfd, reinterpret_cast<sockaddr*>(&clientAddr), &addrLen);
        if (clientSockfd < 0)
        {
            ui::Log(Lv::Warn, std::string("Accept failed: ") + strerror(errno));
            continue;
        }
        // 將連線封裝成 Session 物件
        auto session = std::make_shared<Session>(clientSockfd, i, m_port);
        {
            std::lock_guard<std::mutex> lock(m_sessionMutex);
            m_vec_sessions.push_back(session);
        }

        //create thread by session
        m_vec_threads.emplace_back([this, session, clientAddr]()
        {
            // start login
            std::string buffer;
            int recvSize = 0;
            int recvMsgError = 0;
            auto start = std::chrono::steady_clock::now();
            ui::Log(Lv::Session, "[Session " + std::to_string(session->GetSessionNum()) + "] connected from " +
                                     inet_ntoa(clientAddr.sin_addr));
            {
                std::lock_guard<std::mutex> lock(m_hdrMutex);
                m_hdr.MsgSeqNum = 0;
            }
            currentState = State::Logging;
            currentOrderState = OrderState::Idle;

            // Market feed: started on logon, stopped when this session thread ends.
            struct MarketFeed {
                std::atomic<bool> running{false};
                std::thread thread;
                ~MarketFeed() { running = false; if (thread.joinable()) thread.join(); }
            } feed;
            auto startFeed = [&]() {
                if (feed.running) return;
                feed.running = true;
                feed.thread = std::thread([this, session, &feed]() { MarketLoop(session, feed.running); });
                ui::Log(Lv::Market, "market data feed started (35=W every " + std::to_string(m_tick_ms) + " ms)");
            };

            // use a delay thread to check if logno finished in 60 seconds
            std::thread delayed_thread([this, session]()
            {
                if (!isLogon)
                    std::this_thread::sleep_for(std::chrono::milliseconds(60000));
                if (!isLogon) {
                    ui::Log(Lv::Warn, "[Session " + std::to_string(session->GetSessionNum()) + "] no Logon within 60 s");
                    return 0;
                }
                else {
                    char buffer[32] = {0};
                    while(1)
                    {
                        EnableHeartBeat();
                        sleep(m_heartbeat_interval_sec * 2);
                        if (m_en_hb) {
                            session->SendMsg([&](uint16_t seq) { return m_messenger.Make0(buffer, Hdr(), seq); });
                            {
                                std::lock_guard<std::mutex> lock(m_sessionMutex);
                                m_sent_0_cnt++;
                            }
                        }
                        if (m_rcv) {
                            session->SendMsg([&](uint16_t seq) { return m_messenger.Make1(buffer, Hdr(), seq); });
                        }
                    }
                }
            });
            delayed_thread.detach();

            auto send = [&](const std::function<std::string(uint16_t)>& build) { session->SendMsg(build); };

            while (1) {
                recvSize = session->RecvSinglePacket(buffer);
                m_rcv = false;
                if (recvSize <= 0)
                {
                    ui::Log(Lv::Warn, "[Session " + std::to_string(session->GetSessionNum()) + "] connection closed by peer");
                    return 0;
                }
                ui::LogPacket(Lv::Rx, buffer);
                {
                    std::lock_guard<std::mutex> lock(m_hdrMutex);
                    recvMsgError = m_messenger.SearchMsgType(buffer, m_hdr, m_2_data, m_3_data);
                }
                if (recvMsgError)
                    ui::Log(Lv::Debug, "SearchMsgType returned " + std::to_string(recvMsgError));
                const FIXhdr_t hdr = Hdr();

                switch (currentState) {
                    case State::Logging:
                        if (hdr.MessageType == order.FIXMsgType["Logon"] && !recvMsgError) {
                            m_rcv = true;
                            if (m_messenger.ReceiveA(buffer, &m_A_data)) {
                                SetHeartBeatIntervalInSec(m_A_data.HeartBtInt);
                                send([&](uint16_t seq) { return m_messenger.MakeA(buffer, hdr, seq); });
                                isLogon = true;
                                ui::Log(Lv::Session, "Logon accepted, HeartBtInt=" + std::to_string(m_A_data.HeartBtInt) + "s");
                                SetState(State::Ordering);
                                startFeed();
                            } else {
                                send([&](uint16_t seq) { return m_messenger.Make5(buffer, hdr, seq); });
                                start = std::chrono::steady_clock::now();
                                SetState(State::SendingLogout);
                            }
                        }
                        else if ((hdr.MessageType == order.FIXMsgType["Heartbeat"] && !recvMsgError)) {
                            m_rcv = true;
                            SetState(State::Logging);
                        }
                        else if (hdr.MessageType == order.FIXMsgType["TestRequest"] && !recvMsgError) {
                            m_rcv = true;
                            send([&](uint16_t seq) { return m_messenger.Make0(buffer, hdr, seq); });
                            SetState(State::Logging);
                        }
                        else if (hdr.MessageType == order.FIXMsgType["ResendRequest"] && !recvMsgError) {
                            m_rcv = true;
                            send([&](uint16_t seq) { return m_messenger.Make2resp(buffer, hdr, seq); });
                            SetState(State::Logging);
                        }
                        else if (hdr.MessageType == order.FIXMsgType["RejectSession"] && !recvMsgError) {
                            m_rcv = true;
                            send([&](uint16_t seq) { return m_messenger.Make3resp(buffer, hdr, seq); });
                            SetState(State::Logging);
                        }
                        else if (hdr.MessageType == order.FIXMsgType["Logout"]) {
                            m_rcv = true;
                            send([&](uint16_t seq) { return m_messenger.Make5(buffer, hdr, seq); });
                            SetState(State::Idle);
                            return 1;
                        }
                        else if (recvMsgError == 2) {
                            m_rcv = true;
                            send([&](uint16_t seq) { return m_messenger.Make2(&m_2_data, hdr, seq); });
                            SetState(State::Logging);
                        }
                        else if (recvMsgError == 3) {
                            m_rcv = true;
                            send([&](uint16_t seq) { return m_messenger.Make3(&m_3_data, hdr, seq); });
                            SetState(State::Logging);
                        }
                        else if (recvMsgError == 5) {
                            m_rcv = true;
                            send([&](uint16_t seq) { return m_messenger.Make5(buffer, hdr, seq); });
                            start = std::chrono::steady_clock::now();
                            SetState(State::SendingLogout);
                        }
                        else {
                            send([&](uint16_t seq) { return m_messenger.Make5(buffer, hdr, seq); });
                            start = std::chrono::steady_clock::now();
                            ui::Log(Lv::Warn, "unexpected message while logging on");
                            SetState(State::SendingLogout);
                        }
                        break;
                    case State::Ordering:
                        if (hdr.MessageType == order.FIXMsgType["NewOrder"] ||
                            hdr.MessageType == order.FIXMsgType["OrderCancel"] ||
                            hdr.MessageType == order.FIXMsgType["OrderCancelRequest"] ||
                            hdr.MessageType == order.FIXMsgType["OrderStatus"]) {
                            m_rcv = true;
                            if (m_messenger.ReceiveOrder(buffer, hdr.MessageType)) {
                                HandleOrder(session, buffer);
                            } else {
                                send([&](uint16_t seq) { return m_messenger.Make9(buffer, hdr, seq); });
                            }
                        }
                        else if ((hdr.MessageType == order.FIXMsgType["Heartbeat"] && !recvMsgError)) {
                            m_rcv = true;
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
                            feed.running = false;
                            send([&](uint16_t seq) { return m_messenger.Make5(buffer, hdr, seq); });
                            ui::Log(Lv::Session, "client logged out");
                            SetState(State::Idle);
                            return 1;
                        }
                        else if (recvMsgError == 2) {
                            m_rcv = true;
                            send([&](uint16_t seq) { return m_messenger.Make2(&m_2_data, hdr, seq); });
                        }
                        else if (recvMsgError == 3) {
                            m_rcv = true;
                            send([&](uint16_t seq) { return m_messenger.Make3(&m_3_data, hdr, seq); });
                        }
                        else {
                            feed.running = false;
                            send([&](uint16_t seq) { return m_messenger.Make5(buffer, hdr, seq); });
                            start = std::chrono::steady_clock::now();
                            ui::Log(Lv::Warn, "unexpected message while ordering");
                            SetState(State::SendingLogout);
                        }
                        break;
                    case State::SendingLogout:
                        if (hdr.MessageType == order.FIXMsgType["Logout"]) {
                            m_rcv = true;
                            start = std::chrono::steady_clock::now();
                            SetState(State::Idle);
                            return 1;
                        }
                        else if ((hdr.MessageType == order.FIXMsgType["Heartbeat"] && !recvMsgError) || recvMsgError == 3 || recvMsgError == 2) {
                            m_rcv = true;
                        }
                        else if (hdr.MessageType == order.FIXMsgType["TestRequest"] && !recvMsgError) {
                            m_rcv = true;
                            send([&](uint16_t seq) { return m_messenger.Make0(buffer, hdr, seq); });
                        }
                        else {
                            auto end = std::chrono::steady_clock::now();
                            std::chrono::duration<double> elapsed = end - start;
                            if (elapsed.count() > 5) {
                                ui::Log(Lv::Warn, "no Logout reply within 5 s, closing");
                                SetState(State::Idle);
                                return 1;
                            }
                        }
                        break;
                    default:
                        m_rcv = false;
                        break;
                }
            }
        });
    }

    for(auto& t : m_vec_threads)
    {
        if (t.joinable())
            t.join();
    }
    ui::Log(Lv::Info, "all sessions finished, exchange closed");

    return 0;
}

static void PrintUsage()
{
    std::cout << "Usage: TaifexOrderServer [--port N] [--sessions N] [--tick-ms N] [--board-sec N] [--raw] [--verbose] [--no-color]\n"
                 "  --port N       listen port (default 9000)\n"
                 "  --sessions N   number of client sessions to accept (default 1)\n"
                 "  --tick-ms N    market data tick interval in ms (default 1000)\n"
                 "  --board-sec N  print the quote board every N seconds, 0 = off (default 5)\n"
                 "  --raw          print every FIX packet as a tag table\n"
                 "  --verbose      debug output, including every MarketData (W) packet\n"
                 "  --no-color     disable ANSI colours\n";
}

int main(int argc, char* argv[])
{
    int port = 9000, sessions = 1, tickMs = 1000, boardSec = 5;
    for (int i = 1; i < argc; i++) {
        std::string a = argv[i];
        auto next = [&](int& dst) {
            if (i + 1 >= argc) { PrintUsage(); std::exit(EXIT_FAILURE); }
            dst = std::atoi(argv[++i]);
        };
        if (a == "--port") next(port);
        else if (a == "--sessions") next(sessions);
        else if (a == "--tick-ms") next(tickMs);
        else if (a == "--board-sec") next(boardSec);
        else if (a == "--raw") ui::Cfg().raw = true;
        else if (a == "--verbose") ui::Cfg().verbose = true;
        else if (a == "--no-color") ui::Cfg().color = false;
        else { PrintUsage(); return a == "--help" || a == "-h" ? 0 : 1; }
    }

    TaifexOrderServer server(port, sessions);
    server.SetTickMs(std::max(50, tickMs));
    server.SetBoardSec(boardSec);
    server.Start();
}

// ConsoleUI.h - header-only, thread-safe console helpers shared by
// PseudoServer and PseudoClient. Presentation only: nothing here touches
// FIX encoding/decoding behaviour.
#ifndef HFST_CONSOLE_UI_H
#define HFST_CONSOLE_UI_H

#include <chrono>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <unistd.h>

namespace ui {

struct Config {
    bool color   = false;  // ANSI colours (auto: only when stdout is a TTY)
    bool verbose = false;  // print DEBUG lines
    bool raw     = false;  // print full FIX tag tables instead of one-liners
    bool prompt  = false;  // a "> " prompt is currently displayed
};

inline Config& Cfg()
{
    static Config c = [] { Config x; x.color = isatty(STDOUT_FILENO); return x; }();
    return c;
}

inline std::mutex& Mtx()
{
    static std::mutex m;
    return m;
}

namespace ansi {
constexpr const char* Reset   = "\033[0m";
constexpr const char* Bold    = "\033[1m";
constexpr const char* Dim     = "\033[2m";
constexpr const char* Red     = "\033[31m";
constexpr const char* Green   = "\033[32m";
constexpr const char* Yellow  = "\033[33m";
constexpr const char* Blue    = "\033[34m";
constexpr const char* Magenta = "\033[35m";
constexpr const char* Cyan    = "\033[36m";
constexpr const char* White   = "\033[37m";
}  // namespace ansi

// Returns the escape code only when colour is enabled.
inline std::string C(const char* code) { return Cfg().color ? code : ""; }
inline std::string Paint(const std::string& s, const char* code)
{
    return Cfg().color ? std::string(code) + s + ansi::Reset : s;
}

enum class Lv { Info, Session, Tx, Rx, Order, Fill, Reject, Warn, Market, Debug };

inline const char* LvTag(Lv lv)
{
    switch (lv) {
        case Lv::Info:    return "INFO   ";
        case Lv::Session: return "SESSION";
        case Lv::Tx:      return "TX     ";
        case Lv::Rx:      return "RX     ";
        case Lv::Order:   return "ORDER  ";
        case Lv::Fill:    return "FILL   ";
        case Lv::Reject:  return "REJECT ";
        case Lv::Warn:    return "WARN   ";
        case Lv::Market:  return "MARKET ";
        case Lv::Debug:   return "DEBUG  ";
    }
    return "       ";
}

inline const char* LvColor(Lv lv)
{
    switch (lv) {
        case Lv::Info:    return ansi::White;
        case Lv::Session: return ansi::Blue;
        case Lv::Tx:      return ansi::Cyan;
        case Lv::Rx:      return ansi::Magenta;
        case Lv::Order:   return ansi::Yellow;
        case Lv::Fill:    return ansi::Green;
        case Lv::Reject:  return ansi::Red;
        case Lv::Warn:    return ansi::Red;
        case Lv::Market:  return ansi::White;
        case Lv::Debug:   return ansi::Dim;
    }
    return ansi::Reset;
}

inline std::string Now()
{
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    int ms = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(
                                  now.time_since_epoch()).count() % 1000);
    std::tm tmv;
    localtime_r(&t, &tmv);
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%02d:%02d:%02d.%03d", tmv.tm_hour, tmv.tm_min, tmv.tm_sec, ms);
    return buf;
}

// Must be called with Mtx() held.
inline void ClearPromptLocked()
{
    if (Cfg().prompt)
        std::cout << (Cfg().color ? "\r\033[K" : "\n");
}

inline void RedrawPromptLocked()
{
    if (Cfg().prompt)
        std::cout << C(ansi::Bold) << "> " << C(ansi::Reset) << std::flush;
}

inline void Log(Lv lv, const std::string& msg)
{
    if (lv == Lv::Debug && !Cfg().verbose)
        return;
    std::lock_guard<std::mutex> lock(Mtx());
    ClearPromptLocked();
    std::cout << C(ansi::Dim) << Now() << C(ansi::Reset) << ' '
              << C(LvColor(lv)) << C(ansi::Bold) << '[' << LvTag(lv) << ']' << C(ansi::Reset) << ' '
              << C(LvColor(lv)) << msg << C(ansi::Reset) << '\n';
    RedrawPromptLocked();
    std::cout << std::flush;
}

// Print several lines atomically (no interleaving with other threads).
inline void Block(const std::vector<std::string>& lines)
{
    std::lock_guard<std::mutex> lock(Mtx());
    ClearPromptLocked();
    for (const auto& l : lines)
        std::cout << l << '\n';
    RedrawPromptLocked();
    std::cout << std::flush;
}

inline void ShowPrompt()
{
    std::lock_guard<std::mutex> lock(Mtx());
    Cfg().prompt = true;
    RedrawPromptLocked();
}

inline void HidePrompt()
{
    std::lock_guard<std::mutex> lock(Mtx());
    Cfg().prompt = false;
}

/* ------------------------------ FIX helpers ------------------------------ */

inline std::vector<std::pair<std::string, std::string>> SplitFIX(const std::string& buf)
{
    std::vector<std::pair<std::string, std::string>> out;
    size_t pos = 0;
    while (pos < buf.size()) {
        size_t next = buf.find('\x01', pos);
        if (next == std::string::npos)
            next = buf.size();
        std::string tv = buf.substr(pos, next - pos);
        size_t eq = tv.find('=');
        if (eq != std::string::npos)
            out.emplace_back(tv.substr(0, eq), tv.substr(eq + 1));
        pos = next + 1;
    }
    return out;
}

inline std::string FIXTagName(const std::string& tag)
{
    static const std::map<std::string, std::string> names = {
        {"1", "Account"},          {"6", "AvgPx"},            {"7", "BeginSeqNo"},
        {"8", "BeginString"},      {"9", "BodyLength"},       {"10", "CheckSum"},
        {"11", "ClOrdID"},         {"14", "CumQty"},          {"16", "EndSeqNo"},
        {"17", "ExecID"},          {"31", "LastPx"},          {"32", "LastQty"},
        {"34", "MsgSeqNum"},       {"35", "MsgType"},         {"36", "NewSeqNo"},
        {"37", "OrderID"},         {"38", "OrderQty"},        {"39", "OrdStatus"},
        {"40", "OrdType"},         {"41", "OrigClOrdID"},     {"44", "Price"},
        {"45", "RefSeqNum"},       {"49", "SenderCompID"},    {"52", "SendingTime"},
        {"54", "Side"},            {"55", "Symbol"},          {"56", "TargetCompID"},
        {"58", "Text"},            {"59", "TimeInForce"},     {"60", "TransactTime"},
        {"95", "RawDataLength"},   {"96", "RawData"},         {"98", "EncryptMethod"},
        {"102", "CxlRejReason"},   {"103", "OrdRejReason"},   {"108", "HeartBtInt"},
        {"112", "TestReqID"},      {"123", "GapFillFlag"},    {"150", "ExecType"},
        {"151", "LeavesQty"},      {"262", "MDReqID"},        {"268", "NoMDEntries"},
        {"269", "MDEntryType"},    {"270", "MDEntryPx"},      {"271", "MDEntrySize"},
        {"371", "RefTagID"},       {"372", "RefMsgType"},     {"373", "SessionRejectReason"},
        {"380", "BusinessRejectReason"},                      {"434", "CxlRejResponseTo"},
        {"10000", "TwseIvacnoFlag"}, {"10001", "TwseOrdType"}, {"10002", "TwseExCode"},
        {"10004", "TwseRejStaleOrd"},
    };
    auto it = names.find(tag);
    return it == names.end() ? "?" : it->second;
}

inline std::string MsgTypeName(const std::string& t)
{
    static const std::map<std::string, std::string> names = {
        {"A", "Logon"}, {"0", "Heartbeat"}, {"1", "TestRequest"}, {"2", "ResendRequest"},
        {"3", "Reject"}, {"4", "SequenceReset"}, {"5", "Logout"}, {"D", "NewOrderSingle"},
        {"F", "OrderCancelRequest"}, {"G", "OrderCancelReplaceRequest"}, {"H", "OrderStatusRequest"},
        {"8", "ExecutionReport"}, {"9", "OrderCancelReject"}, {"j", "BusinessMessageReject"},
        {"W", "MarketDataSnapshot"},
    };
    auto it = names.find(t);
    return it == names.end() ? "Unknown" : it->second;
}

inline std::string SideName(char s) { return s == '1' ? "BUY" : s == '2' ? "SELL" : std::string(1, s); }

inline std::string ExecTypeName(char e)
{
    switch (e) {
        case '0': return "NEW";
        case '4': return "CANCELED";
        case '5': return "REPLACED";
        case '8': return "REJECTED";
        case 'F': return "TRADE";
        case 'I': return "STATUS";
    }
    return std::string(1, e);
}

inline std::string OrdStatusName(char s)
{
    switch (s) {
        case '0': return "NEW";
        case '1': return "PARTIAL";
        case '2': return "FILLED";
        case '4': return "CANCELED";
        case '8': return "REJECTED";
    }
    return std::string(1, s);
}

// Human readable meaning of enumerated values, empty when not applicable.
inline std::string FIXValueHint(const std::string& tag, const std::string& v)
{
    if (v.empty()) return "";
    if (tag == "35") return MsgTypeName(v);
    if (tag == "54") return SideName(v[0]);
    if (tag == "150") return ExecTypeName(v[0]);
    if (tag == "39") return OrdStatusName(v[0]);
    if (tag == "40") return v == "1" ? "Market" : v == "2" ? "Limit" : "";
    if (tag == "59") return v == "0" ? "Day" : v == "3" ? "IOC" : v == "4" ? "FOK" : "";
    if (tag == "269") return v == "0" ? "Bid" : v == "1" ? "Offer" : v == "2" ? "Trade" : "";
    if (tag == "434") return v == "1" ? "CancelRequest" : v == "2" ? "CancelReplace" : "";
    return "";
}

// SOH shown as '|', e.g. 8=FIX.4.4|9=72|35=A|...
inline std::string OneLineFIX(const std::string& buf)
{
    std::string s;
    s.reserve(buf.size());
    for (char c : buf) {
        if (c == '\x01') s += '|';
        else if (c == '\0') s += "\\0";
        else s += c;
    }
    return s;
}

// Multi-line table: "  tag  name  value  (meaning)".
inline std::vector<std::string> PrettyFIX(const std::string& buf, const std::string& indent = "    ")
{
    std::vector<std::string> lines;
    for (const auto& kv : SplitFIX(buf)) {
        std::ostringstream os;
        std::string hint = FIXValueHint(kv.first, kv.second);
        std::string value = OneLineFIX(kv.second);
        os << indent << C(ansi::Dim) << std::setw(5) << std::right << kv.first << C(ansi::Reset) << "  "
           << std::setw(20) << std::left << FIXTagName(kv.first) << ' ' << value;
        if (!hint.empty())
            os << C(ansi::Dim) << "  (" << hint << ")" << C(ansi::Reset);
        lines.push_back(os.str());
    }
    return lines;
}

inline std::string FIXField(const std::string& buf, const std::string& tag)
{
    for (const auto& kv : SplitFIX(buf))
        if (kv.first == tag) return kv.second;
    return "";
}

// "D NewOrderSingle seq=3 len=206 cks=087"
inline std::string FIXSummary(const std::string& buf)
{
    std::string t = FIXField(buf, "35");
    std::ostringstream os;
    os << std::setw(2) << std::left << t << ' ' << std::setw(19) << std::left << MsgTypeName(t)
       << " seq=" << FIXField(buf, "34") << " 9=" << FIXField(buf, "9") << " 10=" << FIXField(buf, "10")
       << " (" << buf.size() << " bytes)";
    return os.str();
}

// Log a packet: one line by default, plus the full tag table in raw mode.
inline void LogPacket(Lv lv, const std::string& buf)
{
    Log(lv, FIXSummary(buf));
    if (Cfg().raw) {
        std::vector<std::string> lines = PrettyFIX(buf);
        lines.insert(lines.begin(), "    " + C(ansi::Dim) + OneLineFIX(buf) + C(ansi::Reset));
        Block(lines);
    }
}

inline std::string Fmt(double v, int prec = 2)
{
    std::ostringstream os;
    os << std::fixed << std::setprecision(prec) << v;
    return os.str();
}

}  // namespace ui

#endif

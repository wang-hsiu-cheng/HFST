#include "TaifexORGenerator.h"

std::string TaifexORGenerator::SerializeHeader(const FIXhdr_t& _msg) {
    std::ostringstream oss;
    oss << "35=" << _msg.MessageType   << '\x01';
    oss << "34=" << _msg.MsgSeqNum     << '\x01';
    oss << "49=" << _msg.SenderCompID  << '\x01';
    oss << "56=" << _msg.TargetCompID  << '\x01';
    oss << "52=" << util.GetMsg_time_tStr(_msg.SendingTime) << '\x01';
    return oss.str();
}

std::string TaifexORGenerator::SerializeA(const FIX_A_t& _msg) {
    std::ostringstream oss;
    oss << SerializeHeader(_msg.Header);
    oss << "98="  << _msg.EncryptMethod   << '\x01';
    oss << "108="  << _msg.HeartBtInt    << '\x01';
    oss << "95=" << _msg.RawDataLength       << '\x01';
    oss << "96=" << _msg.RawData     << '\x01';
    return oss.str();
}

std::string TaifexORGenerator::Serialize0(const FIX_0_t& _msg) {
    std::ostringstream oss;
    oss << SerializeHeader(_msg.Header);
    oss << "112="  << _msg.TestReqID   << '\x01';
    return oss.str();
}

std::string TaifexORGenerator::Serialize1(const FIX_1_t& _msg) {
    std::ostringstream oss;
    oss << SerializeHeader(_msg.Header);
    oss << "112="  << _msg.TestReqID   << '\x01';
    return oss.str();
}

std::string TaifexORGenerator::Serialize2(const FIX_2_t& _msg) {
    std::ostringstream oss;
    oss << SerializeHeader(_msg.Header);
    oss << "7="  << _msg.BeginSeqNo   << '\x01';
    oss << "16="  << _msg.EndSeqNo    << '\x01';
    return oss.str();
}

std::string TaifexORGenerator::Serialize3(const FIX_3_t& _msg) {
    std::ostringstream oss;
    oss << SerializeHeader(_msg.Header);
    oss << "45="  << _msg.RefSeqNum   << '\x01';
    oss << "371="  << _msg.RefTagID    << '\x01';
    oss << "372=" << _msg.RefMsgType       << '\x01';
    oss << "373=" << _msg.SessionRejectReason     << '\x01';
    oss << "58=" << _msg.Text     << '\x01';
    return oss.str();
}

std::string TaifexORGenerator::Serialize4(const FIX_4_t& _msg) {
    std::ostringstream oss;
    oss << SerializeHeader(_msg.Header);
    oss << "123="  << _msg.GapFillFlag   << '\x01';
    oss << "36="  << _msg.NewSeqNo    << '\x01';
    return oss.str();
}

std::string TaifexORGenerator::Serialize5(const FIX_5_t& _msg) {
    std::ostringstream oss;
    oss << SerializeHeader(_msg.Header);
    oss << "58="  << _msg.Text   << '\x01';
    return oss.str();
}

std::string TaifexORGenerator::SerializeD(const FIX_D_t& _msg) {
    std::ostringstream oss;
    oss << SerializeHeader(_msg.Header);
    oss << "11="  << _msg.ClOrdID   << '\x01';
    oss << "37="  << _msg.OrderID    << '\x01';
    oss << "1=" << _msg.Account       << '\x01';
    oss << "55=" << _msg.Symbol     << '\x01';
    oss << "54="  << _msg.Side   << '\x01';
    oss << "60="  << util.GetMsg_time_tStr(_msg.TransactTime)    << '\x01';
    oss << "38=" << _msg.OrderQty       << '\x01';
    oss << "40=" << _msg.OrderType     << '\x01';
    oss << "59="  << _msg.TimeInFource   << '\x01';
    oss << "44="  << _msg.Price    << '\x01';
    oss << "10000=" << _msg.TwselvacnoFlag       << '\x01';
    oss << "10001=" << _msg.TwseOrdType     << '\x01';
    oss << "10002=" << _msg.TwseExCode       << '\x01';
    oss << "10004=" << _msg.TwseRejStaleOrd     << '\x01';
    return oss.str();
}

std::string TaifexORGenerator::SerializeF(const FIX_F_t& _msg) {
    std::ostringstream oss;
    oss << SerializeHeader(_msg.Header);
    oss << "11="  << _msg.ClOrdID   << '\x01';
    oss << "41="  << _msg.OrigClOrdID    << '\x01';
    oss << "37=" << _msg.OrderID       << '\x01';
    oss << "1=" << _msg.Account     << '\x01';
    oss << "55="  << _msg.Symbol   << '\x01';
    oss << "54="  << _msg.Side    << '\x01';
    oss << "60=" << util.GetMsg_time_tStr(_msg.TransactTime)       << '\x01';
    oss << "10000=" << _msg.TwselvacnoFlag     << '\x01';
    oss << "10002=" << _msg.TwseExCode       << '\x01';
    oss << "10004=" << _msg.TwseRejStaleOrd     << '\x01';
    return oss.str();
}

std::string TaifexORGenerator::SerializeG(const FIX_G_t& _msg) {
    std::ostringstream oss;
    oss << SerializeHeader(_msg.Header);
    oss << "11="  << _msg.ClOrdID   << '\x01';
    oss << "41="  << _msg.OrigClOrdID    << '\x01';
    oss << "37=" << _msg.OrderID       << '\x01';
    oss << "1=" << _msg.Account     << '\x01';
    oss << "55="  << _msg.Symbol   << '\x01';
    oss << "54="  << _msg.Side    << '\x01';
    oss << "60=" << util.GetMsg_time_tStr(_msg.TransactTime)       << '\x01';
    oss << "38=" << _msg.OrderQty     << '\x01';
    oss << "44="  << _msg.Price   << '\x01';
    oss << "10000="  << _msg.TwselvacnoFlag    << '\x01';
    oss << "10001=" << _msg.TwseOrdType       << '\x01';
    oss << "10002=" << _msg.TwseExCode     << '\x01';
    return oss.str();
}

std::string TaifexORGenerator::SerializeH(const FIX_H_t& _msg) {
    std::ostringstream oss;
    oss << SerializeHeader(_msg.Header);
    oss << "11="  << _msg.ClOrdID   << '\x01';
    oss << "37="  << _msg.OrderID    << '\x01';
    oss << "55=" << _msg.Symbol       << '\x01';
    oss << "54=" << _msg.Side     << '\x01';
    oss << "10000=" << _msg.TwselvacnoFlag       << '\x01';
    oss << "10002=" << _msg.TwseExCode     << '\x01';
    return oss.str();
}

std::string TaifexORGenerator::Serialize8(const FIX_8_t& _msg) {
    std::ostringstream oss;
    oss << SerializeHeader(_msg.Header);
    oss << "37="  << _msg.OrderID    << '\x01';
    oss << "11="  << _msg.ClOrdID    << '\x01';
    oss << "41=" << _msg.OrigClOrdID << '\x01';
    oss << "17=" << _msg.ExecID      << '\x01';
    oss << "150="  << _msg.ExecType  << '\x01';
    oss << "39="  << _msg.OrdStatus  << '\x01';
    oss << "103=" << _msg.OrdRejReason       << '\x01';
    oss << "1=" << _msg.Account      << '\x01';
    oss << "55="  << _msg.Symbol     << '\x01';
    oss << "54="  << _msg.Side       << '\x01';
    oss << "60=" << util.GetMsg_time_tStr(_msg.TransactTime)       << '\x01';
    oss << "38=" << _msg.OrderQty    << '\x01';
    oss << "40="  << _msg.OrdType    << '\x01';
    oss << "59="  << _msg.TimeInForce    << '\x01';
    oss << "44=" << _msg.Price       << '\x01';
    oss << "32=" << _msg.LastQty     << '\x01';
    oss << "31=" << _msg.LastPx      << '\x01';
    oss << "151="  << _msg.LeavesQty << '\x01';
    oss << "14="  << _msg.CumQty     << '\x01';
    oss << "6=" << _msg.AvgPx        << '\x01';
    oss << "58=" << _msg.Text        << '\x01';
    oss << "10000="  << _msg.TwseIvacnoFlag    << '\x01';
    oss << "10001=" << _msg.TwseOrdType       << '\x01';
    oss << "10002=" << _msg.TwseExCode     << '\x01';
    return oss.str();
}

std::string TaifexORGenerator::Serialize9(const FIX_9_t& _msg) {
    std::ostringstream oss;
    oss << SerializeHeader(_msg.Header);
    oss << "37="  << _msg.OrderID   << '\x01';
    oss << "11="  << _msg.ClOrdID    << '\x01';
    oss << "41=" << _msg.OrigClOrdID       << '\x01';
    oss << "39=" << _msg.OrdStatus     << '\x01';
    oss << "1="  << _msg.Account   << '\x01';
    oss << "60="  << util.GetMsg_time_tStr(_msg.TransactTime)    << '\x01';
    oss << "434=" << _msg.CxlRejResponseTo       << '\x01';
    oss << "102=" << _msg.CxlRejReason     << '\x01';
    oss << "58="  << _msg.Text   << '\x01';
    return oss.str();
}

std::string TaifexORGenerator::Serializej(const FIX_j_t& _msg) {
    std::ostringstream oss;
    oss << SerializeHeader(_msg.Header);
    oss << "45="  << _msg.RefSeqNum   << '\x01';
    oss << "372=" << _msg.RefMsgType    << '\x01';
    oss << "380=" << _msg.BusinessRejectReason       << '\x01';
    oss << "58="  << _msg.Text     << '\x01';
    return oss.str();
}

#include "ClientMessenger.h"

string ClientMessenger::MakeA(const std::string &_buf, FIXhdr_t _header, uint16_t _clientSeqNum)
{
    std::string m_A_str, newSenderID;
    FIX_A_t m_A_data;
    int appendNo, password;

    srand (( unsigned ) time ( NULL ));
    appendNo = rand() % 999 + 1;
    password = gen.GetPassword(_header.SenderCompID);
    
    m_A_data.EncryptMethod = 0;
    m_A_data.HeartBtInt = 10;
    m_A_data.RawData = appendNo * (password + 100);
    m_A_data.RawDataLength = 5;
    // m_A_str = std::string(reinterpret_cast<const char*>(&m_A_data), sizeof(m_A_data));
    _header.BeginString = "FIX.4.4";
    _header.MessageType = "A";
    _header.MsgSeqNum = _clientSeqNum;
    newSenderID = _header.TargetCompID;
    _header.TargetCompID = _header.SenderCompID;
    _header.SenderCompID = newSenderID;
    _header.SendingTime = util.GetMsg_time_t();
    m_A_data.Header = _header;

    m_A_str = gen.SerializeA(m_A_data);
    _header.BodyLength = std::to_string(m_A_str.size());
    m_A_str = "8=FIX.4.4" + std::string(1, SOH) + "9=" + std::string(_header.BodyLength) + std::string(1, SOH) + m_A_str;
    m_A_str.append("10=" + util.FIXComputeCheckSum(m_A_str) + std::string(1, SOH));

    return m_A_str;
}

string ClientMessenger::Make0(const std::string &_buf, FIXhdr_t _header, uint16_t _clientSeqNum) 
{
    std::string m_0_str, newSenderID, test_reqID;
    FIX_0_t m_0_data;

    test_reqID = DecodeMessage(_buf, "2");

    m_0_data.TestReqID = test_reqID;
    // m_0_str = std::string(reinterpret_cast<const char*>(&m_0_data), sizeof(m_0_data));

    _header.BeginString = "FIX.4.4";
    _header.MessageType = "0";
    _header.MsgSeqNum = _clientSeqNum;
    newSenderID = _header.TargetCompID;
    _header.TargetCompID = _header.SenderCompID;
    _header.SenderCompID = newSenderID;
    _header.SendingTime = util.GetMsg_time_t();
    m_0_data.Header = _header;

    m_0_str = gen.Serialize0(m_0_data);
    _header.BodyLength = std::to_string(m_0_str.size());
    m_0_str = "8=FIX.4.4" + std::string(1, SOH) + "9=" + std::string(_header.BodyLength) + std::string(1, SOH) + m_0_str;
    m_0_str.append("10=" + util.FIXComputeCheckSum(m_0_str) + std::string(1, SOH));

    return m_0_str;
}

string ClientMessenger::Make1(const std::string &_buf, FIXhdr_t _header, uint16_t _clientSeqNum) 
{
    std::string m_1_str, newSenderID, test_reqID;
    FIX_1_t m_1_data;

    test_reqID = DecodeMessage(_buf, "2");

    m_1_data.TestReqID = test_reqID;
    // m_1_str = std::string(reinterpret_cast<const char*>(&m_1_data), sizeof(m_1_data));

    _header.BeginString = "FIX.4.4";
    _header.MessageType = "1";
    _header.MsgSeqNum = _clientSeqNum;
    newSenderID = _header.TargetCompID;
    _header.TargetCompID = _header.SenderCompID;
    _header.SenderCompID = newSenderID;
    _header.SendingTime = util.GetMsg_time_t();
    m_1_data.Header = _header;
    m_1_str = gen.Serialize1(m_1_data);
    _header.BodyLength = std::to_string(m_1_str.size());
    m_1_str = "8=FIX.4.4" + std::string(1, SOH) + "9=" + std::string(_header.BodyLength) + std::string(1, SOH) + m_1_str;
    m_1_str.append("10=" + util.FIXComputeCheckSum(m_1_str) + std::string(1, SOH));

    return m_1_str;
}

string ClientMessenger::Make2(FIX_2_t *m_2_data, FIXhdr_t _header, uint16_t _clientSeqNum) 
{
    std::string m_2_str, newSenderID;

    // m_2_str = std::string(reinterpret_cast<const char*>(&m_2_data), sizeof(m_2_data));

    _header.BeginString = "FIX.4.4";
    _header.MessageType = "2";
    _header.MsgSeqNum = _clientSeqNum;
    newSenderID = _header.TargetCompID;
    _header.TargetCompID = _header.SenderCompID;
    _header.SenderCompID = newSenderID;
    _header.SendingTime = util.GetMsg_time_t();
    m_2_data->Header = _header;
    m_2_str = gen.Serialize2(*m_2_data);
    _header.BodyLength = std::to_string(m_2_str.size());
    m_2_str = "8=FIX.4.4" + std::string(1, SOH) + "9=" + std::string(_header.BodyLength) + std::string(1, SOH) + m_2_str;
    m_2_str.append("10=" + util.FIXComputeCheckSum(m_2_str) + std::string(1, SOH));

    return m_2_str;
}

string ClientMessenger::Make2resp(const std::string &_buf, FIXhdr_t _header, uint16_t _clientSeqNum) 
{
    return _buf;
}

string ClientMessenger::Make3(FIX_3_t *m_3_data, FIXhdr_t _header, uint16_t _clientSeqNum) 
{
    std::string m_3_str, newSenderID;

    // m_3_str = std::string(reinterpret_cast<const char*>(&m_3_data), sizeof(m_3_data));

    _header.BeginString = "FIX.4.4";
    _header.MessageType = "3";
    _header.MsgSeqNum = _clientSeqNum;
    newSenderID = _header.TargetCompID;
    _header.TargetCompID = _header.SenderCompID;
    _header.SenderCompID = newSenderID;
    _header.SendingTime = util.GetMsg_time_t();
    m_3_data->Header = _header;
    m_3_str = gen.Serialize3(*m_3_data);
    _header.BodyLength = std::to_string(m_3_str.size());
    m_3_str = "8=FIX.4.4" + std::string(1, SOH) + "9=" + std::string(_header.BodyLength) + std::string(1, SOH) + m_3_str;
    m_3_str.append("10=" + util.FIXComputeCheckSum(m_3_str) + std::string(1, SOH));

    return m_3_str;
}

string ClientMessenger::Make3resp(const std::string &_buf, FIXhdr_t _header, uint16_t _clientSeqNum) 
{
    return _buf;
}

string ClientMessenger::Make4(const std::string &_buf, FIXhdr_t _header, uint16_t _clientSeqNum) 
{
    std::string m_4_str, newSenderID;
    FIX_4_t m_4_data;

    m_4_data.GapFillFlag = true;
    m_4_data.NewSeqNo = _clientSeqNum;
    // m_4_str = std::string(reinterpret_cast<const char*>(&m_4_data), sizeof(m_4_data));

    _header.BeginString = "FIX.4.4";
    _header.MessageType = "4";
    _header.MsgSeqNum = _clientSeqNum;
    newSenderID = _header.TargetCompID;
    _header.TargetCompID = _header.SenderCompID;
    _header.SenderCompID = newSenderID;
    _header.SendingTime = util.GetMsg_time_t();
    m_4_data.Header = _header;
    m_4_str = gen.Serialize4(m_4_data);
    _header.BodyLength = std::to_string(m_4_str.size());
    m_4_str = "8=FIX.4.4" + std::string(1, SOH) + "9=" + std::string(_header.BodyLength) + std::string(1, SOH) + m_4_str;
    m_4_str.append("10=" + util.FIXComputeCheckSum(m_4_str) + std::string(1, SOH));

    return m_4_str;
}

string ClientMessenger::Make5(const std::string &_buf, FIXhdr_t _header, uint16_t _clientSeqNum) 
{
    std::string m_5_str, newSenderID;
    FIX_5_t m_5_data;

    m_5_data.Text = "text";
    // m_5_str = std::string(reinterpret_cast<const char*>(&m_5_data), sizeof(m_5_data));

    _header.BeginString = "FIX.4.4";
    _header.MessageType = "5";
    _header.MsgSeqNum = _clientSeqNum;
    newSenderID = _header.TargetCompID;
    _header.TargetCompID = _header.SenderCompID;
    _header.SenderCompID = newSenderID;
    _header.SendingTime = util.GetMsg_time_t();
    m_5_data.Header = _header;
    m_5_str = gen.Serialize5(m_5_data);
    _header.BodyLength = std::to_string(m_5_str.size());
    m_5_str = "8=FIX.4.4" + std::string(1, SOH) + "9=" + std::string(_header.BodyLength) + std::string(1, SOH) + m_5_str;
    m_5_str.append("10=" + util.FIXComputeCheckSum(m_5_str) + std::string(1, SOH));

    return m_5_str;
}

string ClientMessenger::MakeD(const std::string &_buf, FIXhdr_t _header, uint16_t _clientSeqNum) 
{
    std::string m_D_str, newSenderID;
    FIX_D_t m_D_data;
    float unit;
    int lots;

	m_D_data.ClOrdID = "Given by broker";
	m_D_data.OrderID = "Given by broker";
    m_D_data.Account = "Given by broker";
    m_D_data.Symbol = "002330";
    m_D_data.Side = '1';
    m_D_data.TransactTime = util.GetMsg_time_t();
    m_D_data.OrderQty = unit * lots;
    m_D_data.OrderType = '2';
    m_D_data.TimeInFource = '0';
    m_D_data.Price = 10000.0001;
    m_D_data.TwselvacnoFlag = '1';
    m_D_data.TwseOrdType = '0';
    m_D_data.TwseExCode = '0';
    m_D_data.TwseRejStaleOrd = false;
    // m_D_str = std::string(reinterpret_cast<const char*>(&m_D_data), sizeof(m_D_data));

    _header.BeginString = "FIX.4.4";
    _header.MessageType = "D";
    _header.MsgSeqNum = _clientSeqNum;
    newSenderID = _header.TargetCompID;
    _header.TargetCompID = _header.SenderCompID;
    _header.SenderCompID = newSenderID;
    _header.SendingTime = util.GetMsg_time_t();
    m_D_data.Header = _header;
    m_D_str = gen.SerializeD(m_D_data);
    _header.BodyLength = std::to_string(m_D_str.size());
    m_D_str = "8=FIX.4.4" + std::string(1, SOH) + "9=" + std::string(_header.BodyLength) + std::string(1, SOH) + m_D_str;
    m_D_str.append("10=" + util.FIXComputeCheckSum(m_D_str) + std::string(1, SOH));

    return m_D_str;
}

string ClientMessenger::MakeF(const std::string &_buf, FIXhdr_t _header, uint16_t _clientSeqNum) 
{
    std::string m_F_str, newSenderID;
    FIX_F_t m_F_data;

	m_F_data.ClOrdID = "Given by broker";
    m_F_data.OrigClOrdID = "Given by broker";
	m_F_data.OrderID = "Given by broker";
    m_F_data.Account = "Given by broker";
    m_F_data.Symbol = "002330";
    m_F_data.Side = '1';
    m_F_data.TransactTime = util.GetMsg_time_t();
    m_F_data.TwselvacnoFlag = '1';
    m_F_data.TwseExCode = '0';
    m_F_data.TwseRejStaleOrd = false;
    // m_F_str = std::string(reinterpret_cast<const char*>(&m_F_data), sizeof(m_F_data));

    _header.BeginString = "FIX.4.4";
    _header.MessageType = "F";
    _header.MsgSeqNum = _clientSeqNum;
    newSenderID = _header.TargetCompID;
    _header.TargetCompID = _header.SenderCompID;
    _header.SenderCompID = newSenderID;
    _header.SendingTime = util.GetMsg_time_t();
    m_F_data.Header = _header;
    m_F_str = gen.SerializeF(m_F_data);
    _header.BodyLength = std::to_string(m_F_str.size());
    m_F_str = "8=FIX.4.4" + std::string(1, SOH) + "9=" + std::string(_header.BodyLength) + std::string(1, SOH) + m_F_str;
    m_F_str.append("10=" + util.FIXComputeCheckSum(m_F_str) + std::string(1, SOH));

    return m_F_str;
}

string ClientMessenger::MakeG(const std::string &_buf, FIXhdr_t _header, uint16_t _clientSeqNum) 
{
    std::string m_G_str, newSenderID;
    FIX_G_t m_G_data;

	m_G_data.ClOrdID = "Given by broker";
    m_G_data.OrigClOrdID = "Given by broker";
	m_G_data.OrderID = "Given by broker";
    m_G_data.Account = "Given by broker";
    m_G_data.Symbol = "002330";
    m_G_data.Side = '1';
    m_G_data.TransactTime = util.GetMsg_time_t();
    m_G_data.OrderQty = 0;
    m_G_data.Price = 10001.0001;
    m_G_data.TwselvacnoFlag = '1';
    m_G_data.TwseOrdType = '0';
    m_G_data.TwseExCode = '0';
    // m_G_str = std::string(reinterpret_cast<const char*>(&m_G_data), sizeof(m_G_data));

    _header.BeginString = "FIX.4.4";
    _header.MessageType = "G";
    _header.MsgSeqNum = _clientSeqNum;
    newSenderID = _header.TargetCompID;
    _header.TargetCompID = _header.SenderCompID;
    _header.SenderCompID = newSenderID;
    _header.SendingTime = util.GetMsg_time_t();
    m_G_data.Header = _header;
    m_G_str = gen.SerializeG(m_G_data);
    _header.BodyLength = std::to_string(m_G_str.size());
    m_G_str = "8=FIX.4.4" + std::string(1, SOH) + "9=" + std::string(_header.BodyLength) + std::string(1, SOH) + m_G_str;
    m_G_str.append("10=" + util.FIXComputeCheckSum(m_G_str) + std::string(1, SOH));

    return m_G_str;
}

string ClientMessenger::MakeH(const std::string &_buf, FIXhdr_t _header, uint16_t _clientSeqNum) 
{
    std::string m_H_str, newSenderID;
    FIX_H_t m_H_data;

	m_H_data.ClOrdID = "Given by broker";
	m_H_data.OrderID = "Given by broker";
    m_H_data.Symbol = "002330";
    m_H_data.Side = '1';
    m_H_data.TwselvacnoFlag = '1';
    m_H_data.TwseExCode = '0';
    // m_H_str = std::string(reinterpret_cast<const char*>(&m_H_data), sizeof(m_H_data));

    _header.BeginString = "FIX.4.4";
    _header.MessageType = "H";
    _header.MsgSeqNum = _clientSeqNum;
    newSenderID = _header.TargetCompID;
    _header.TargetCompID = _header.SenderCompID;
    _header.SenderCompID = newSenderID;
    _header.SendingTime = util.GetMsg_time_t();
    m_H_data.Header = _header;
    m_H_str = gen.SerializeH(m_H_data);
    _header.BodyLength = std::to_string(m_H_str.size());
    m_H_str = "8=FIX.4.4" + std::string(1, SOH) + "9=" + std::string(_header.BodyLength) + std::string(1, SOH) + m_H_str;
    m_H_str.append("10=" + util.FIXComputeCheckSum(m_H_str) + std::string(1, SOH));

    return m_H_str;
}

string ClientMessenger::getChecksumFromFIX(const std::string& _buf) {
    const std::string tag = "10=";
    size_t pos = _buf.rfind(tag);  // 找最後一個出現的 "10="，通常是最後一欄
    if (pos == std::string::npos || pos + 6 > _buf.size()) {
        return "";  // 找不到，或太短
    }
    size_t end = _buf.find('\x01', pos);  // 找 SOH 結尾
    if (end == std::string::npos) {
        return "";
    }
    std::string checksum = _buf.substr(pos + 3, end - (pos + 3));  // 擷取值
    return checksum;
}

int ClientMessenger::SearchMsgType(const std::string& _buf, FIXhdr_t& _header, FIX_2_t& resendRequest_msg, FIX_3_t& reject_msg)
{
    size_t pos = 0;
    int tagNumber = 0;
    bool needReject = false;
    std::string checkSum = "";
    size_t size;
    checkSum = getChecksumFromFIX(_buf);
    size = _buf.size() - checkSum.size();
    if (checkSum == "" || util.FIXComputeCheckSum(_buf) == checkSum) {
        return 5;
    }
    while (pos < _buf.size() && tagNumber < 7) {
        size_t next = _buf.find(SOH, pos);
        if (next == std::string::npos) 
            return 5;

        std::string tagValue = _buf.substr(pos, next - pos);
        size_t eqPos = tagValue.find('=');
        if (eqPos == std::string::npos) {
            pos = next + 1;
            continue;
        }

        auto clean = [](std::string s) {
            s.erase(std::remove_if(s.begin(), s.end(),
                                [](unsigned char c){ return std::isspace(c) || c == '\x01'; }),
                    s.end());
            return s;
        };
        std::string tag = clean(tagValue.substr(0, eqPos));
        std::string value = clean(tagValue.substr(eqPos + 1));
        tagNumber++;
        std::cout << "tagNumber: " << tagNumber <<", tag: " << tag << ", value: " << value << std::endl;
        if (tagNumber == 1 && tag == "8") {
            _header.BeginString = value;
            if (_header.BeginString != "FIX.4.4") {
                return 5;
            }
        }
        else if (tagNumber == 2 && tag == "9") {
            _header.BodyLength = value;
            if (STR_TO_INT(_header.BodyLength) != _buf.size() - 22) {
                reject_msg.RefTagID = '9';
                reject_msg.SessionRejectReason = 5;
                reject_msg.Text = "";
                needReject = true;
            }
        }
        else if (tagNumber == 3 && tag == "35") {
            _header.MessageType = value;
        }
        else if (tagNumber == 4 && tag == "34") {
            if (STR_TO_INT(value) > _header.MsgSeqNum + 1) {
                std::cout << "message sequence error, sned MsgType = 2" << std::endl;
                resendRequest_msg.BeginSeqNo = _header.MsgSeqNum + 1;
                resendRequest_msg.EndSeqNo = STR_TO_INT(value) - 1;
                return 2;
            }
            else if (STR_TO_INT(value) < _header.MsgSeqNum + 1) {
                return 5;
            }
            _header.MsgSeqNum = STR_TO_INT(value);
        }
        else if (tagNumber == 5 && tag == "49") {
            _header.SenderCompID = value;
        }
        else if (tagNumber == 6 && tag == "56") {
            _header.TargetCompID = value;
        }
        else if (tagNumber == 7 && tag == "52") {
            _header.SendingTime = util.GetMsg_time_t(value);
        }
        else {
            std::cout << "tag order error, send MsgType = 5" << std::endl;
            return 5;
        }
        pos = next + 1;
    }
    if (needReject) {
        reject_msg.RefSeqNum = _header.MsgSeqNum;
        reject_msg.RefMsgType = _header.MessageType;
        return 3;
    }

    return 0;
}

std::string ClientMessenger::DecodeMessage(const std::string &msg, std::string target_tag)
{
    // Split by SOH into tag=value
    size_t pos = 0;
    while (pos < msg.size())
    {
        size_t next = msg.find(SOH, pos);
        if (next == std::string::npos)
            break;

        std::string tagValue = msg.substr(pos, next - pos);
        size_t eqPos = tagValue.find('=');
        if (eqPos != std::string::npos)
        {
            std::string tag = tagValue.substr(0, eqPos);
            std::string value = tagValue.substr(eqPos + 1);

            if (tag == target_tag) { return value; }
        }
        pos = next + 1;
    }
    return "no this tag";
}

bool ClientMessenger::ReceiveA(std::string &msg, FIX_A_t *logonMsg) {
    // Split by SOH into tag=value
    size_t pos = 0;
    while (pos < msg.size())
    {
        size_t next = msg.find(SOH, pos);
        if (next == std::string::npos)
            break;

        std::string tagValue = msg.substr(pos, next - pos);
        size_t eqPos = tagValue.find('=');
        if (eqPos != std::string::npos)
        {
            std::string tag = tagValue.substr(0, eqPos);
            std::string value = tagValue.substr(eqPos + 1);

            if (tag == "8") { logonMsg->Header.BeginString = value; }
            else if (tag == "9") { logonMsg->Header.BodyLength = static_cast<uint16_t>(std::stoi(value)); }
            else if (tag == "35") { logonMsg->Header.MessageType = value; }
            else if (tag == "34") { logonMsg->Header.MsgSeqNum = static_cast<uint32_t>(std::stoul(value)); }
            else if (tag == "49") { logonMsg->Header.SenderCompID = value; }
            else if (tag == "56") { logonMsg->Header.TargetCompID = value; }
            else if (tag == "52") { logonMsg->Header.SendingTime = util.GetMsg_time_t(value); }
            else if (tag == "98") { logonMsg->EncryptMethod = static_cast<uint16_t>(std::stoi(value)); }
            else if (tag == "108") { logonMsg->HeartBtInt = static_cast<uint16_t>(std::stoi(value)); }
            else if (tag == "95") { logonMsg->RawDataLength = static_cast<uint16_t>(std::stoi(value)); }
            else if (tag == "96") { logonMsg->RawData = static_cast<uint16_t>(std::stoi(value)); } 
            else if (tag == "10") { logonMsg->CheckSum = value; }
        }
        pos = next + 1;
    }

    // Check basic validity — must be MsgType A
    if (logonMsg->Header.MessageType != "A")
    {
        std::cerr << "[ERROR] Received message is not Logon (A)." << std::endl;
        return false;
    }

    // Optional: store logonMsg into class member
    // this->lastLogonMsg = logonMsg;

    return true;
}


bool ClientMessenger::ReceiveOrder(std::string &msg, std::string msgType) {
    if (msgType == order.FIXMsgType["NewOrder"]) {
        return true;
    } else if (msgType == order.FIXMsgType["OrderCancelRequest"]) {
        return true;
    } else if (msgType == order.FIXMsgType["OrderCancel"]) {
        return true;
    } else if (msgType == order.FIXMsgType["OrderStatus"]) {
        return true;
    }
    return false;
}
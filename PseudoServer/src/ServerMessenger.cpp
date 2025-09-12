#include "ServerMessenger.h"

string ServerMessenger::MakeA(uint8_t *buf, FIXhdr_t hdr, uint16_t serverSeqNum)
{
    std::string m_A_str, new_senderID;
    FIX_A_t m_A_data;
    int appendNo, password;

    srand (( unsigned ) time ( NULL ));
    appendNo = rand() % 999 + 1;
    password = gen.GetPassword(hdr.SenderCompID);
    
    m_A_data.EncryptMethod = 0;
    m_A_data.HeartBtInt = 10;
    m_A_data.RawData = appendNo * (password + 100);
    m_A_data.RawDataLength = sizeof(FIX_A_t);
    m_A_str = std::string(reinterpret_cast<const char*>(&m_A_data), sizeof(m_A_data));

    hdr.BeginString = "FIX.4.4";
    hdr.BodyLength = std::to_string(m_A_str.size());
    hdr.MessageType = "A";
    hdr.MsgSeqNum = serverSeqNum;
    new_senderID = hdr.TargetCompID;
    hdr.TargetCompID = hdr.SenderCompID;
    hdr.SenderCompID = new_senderID;
    hdr.SendingTime = util.GetMsg_time_t();
    m_A_data.hdr = hdr;
    m_A_str.append(util.FIXComputeCheckSum(m_A_str));

    return m_A_str;
}

string ServerMessenger::Make0(uint8_t *buf, FIXhdr_t hdr, uint16_t serverSeqNum) 
{
    std::string m_0_str, new_senderID, test_reqID;
    FIX_0_t m_0_data;

    test_reqID = DecodeMessage(buf, "2");

    m_0_data.TestReqID = test_reqID;
    m_0_str = std::string(reinterpret_cast<const char*>(&m_0_data), sizeof(m_0_data));

    hdr.BeginString = "FIX.4.4";
    hdr.BodyLength = std::to_string(m_0_str.size());
    hdr.MessageType = "0";
    hdr.MsgSeqNum = serverSeqNum;
    new_senderID = hdr.TargetCompID;
    hdr.TargetCompID = hdr.SenderCompID;
    hdr.SenderCompID = new_senderID;
    hdr.SendingTime = util.GetMsg_time_t();
    m_0_data.hdr = hdr;
    m_0_str.append(util.FIXComputeCheckSum(m_0_str));

    return m_0_str;
}

string ServerMessenger::Make1(uint8_t *buf, FIXhdr_t hdr, uint16_t serverSeqNum) 
{
    std::string m_1_str, new_senderID, test_reqID;
    FIX_1_t m_1_data;

    test_reqID = DecodeMessage(buf, "2");

    m_1_data.TestReqID = test_reqID;
    m_1_str = std::string(reinterpret_cast<const char*>(&m_1_data), sizeof(m_1_data));

    hdr.BeginString = "FIX.4.4";
    hdr.BodyLength = std::to_string(m_1_str.size());
    hdr.MessageType = "1";
    hdr.MsgSeqNum = serverSeqNum;
    new_senderID = hdr.TargetCompID;
    hdr.TargetCompID = hdr.SenderCompID;
    hdr.SenderCompID = new_senderID;
    hdr.SendingTime = util.GetMsg_time_t();
    m_1_data.hdr = hdr;
    m_1_str.append(util.FIXComputeCheckSum(m_1_str));

    return m_1_str;
}

string ServerMessenger::Make2(FIX_2_t *m_2_data, FIXhdr_t hdr, uint16_t serverSeqNum) 
{
    std::string m_2_str, new_senderID;

    m_2_str = std::string(reinterpret_cast<const char*>(&m_2_data), sizeof(m_2_data));

    hdr.BeginString = "FIX.4.4";
    hdr.BodyLength = std::to_string(m_2_str.size());
    hdr.MessageType = "2";
    hdr.MsgSeqNum = serverSeqNum;
    new_senderID = hdr.TargetCompID;
    hdr.TargetCompID = hdr.SenderCompID;
    hdr.SenderCompID = new_senderID;
    hdr.SendingTime = util.GetMsg_time_t();
    m_2_data->hdr = hdr;
    m_2_str.append(util.FIXComputeCheckSum(m_2_str));

    return m_2_str;
}

string ServerMessenger::Make2resp(uint8_t *buf, FIXhdr_t hdr, uint16_t serverSeqNum) 
{
    std::string buf_str(reinterpret_cast<char*>(buf));
    return buf_str;
}

string ServerMessenger::Make3(FIX_3_t *m_3_data, FIXhdr_t hdr, uint16_t serverSeqNum) 
{
    std::string m_3_str, new_senderID;

    m_3_str = std::string(reinterpret_cast<const char*>(&m_3_data), sizeof(m_3_data));

    hdr.BeginString = "FIX.4.4";
    hdr.BodyLength = std::to_string(m_3_str.size());
    hdr.MessageType = "3";
    hdr.MsgSeqNum = serverSeqNum;
    new_senderID = hdr.TargetCompID;
    hdr.TargetCompID = hdr.SenderCompID;
    hdr.SenderCompID = new_senderID;
    hdr.SendingTime = util.GetMsg_time_t();
    m_3_data->hdr = hdr;
    m_3_str.append(util.FIXComputeCheckSum(m_3_str));

    return m_3_str;
}

string ServerMessenger::Make3resp(uint8_t *buf, FIXhdr_t hdr, uint16_t serverSeqNum) 
{
    std::string buf_str(reinterpret_cast<char*>(buf));
    return buf_str;
}

string ServerMessenger::Make4(uint8_t *buf, FIXhdr_t hdr, uint16_t serverSeqNum) 
{
    std::string m_4_str, new_senderID;
    FIX_4_t m_4_data;

    m_4_data.GapFillFlag = true;
    m_4_data.NewSeqNo = serverSeqNum;
    m_4_str = std::string(reinterpret_cast<const char*>(&m_4_data), sizeof(m_4_data));

    hdr.BeginString = "FIX.4.4";
    hdr.BodyLength = std::to_string(m_4_str.size());
    hdr.MessageType = "4";
    hdr.MsgSeqNum = serverSeqNum;
    new_senderID = hdr.TargetCompID;
    hdr.TargetCompID = hdr.SenderCompID;
    hdr.SenderCompID = new_senderID;
    hdr.SendingTime = util.GetMsg_time_t();
    m_4_data.hdr = hdr;
    m_4_str.append(util.FIXComputeCheckSum(m_4_str));

    return m_4_str;
}

string ServerMessenger::Make5(uint8_t *buf, FIXhdr_t hdr, uint16_t serverSeqNum) 
{
    std::string m_5_str, new_senderID;
    FIX_5_t m_5_data;

    m_5_data.Text = "text";
    m_5_str = std::string(reinterpret_cast<const char*>(&m_5_data), sizeof(m_5_data));

    hdr.BeginString = "FIX.4.4";
    hdr.BodyLength = std::to_string(m_5_str.size());
    hdr.MessageType = "5";
    hdr.MsgSeqNum = serverSeqNum;
    new_senderID = hdr.TargetCompID;
    hdr.TargetCompID = hdr.SenderCompID;
    hdr.SenderCompID = new_senderID;
    hdr.SendingTime = util.GetMsg_time_t();
    m_5_data.hdr = hdr;
    m_5_str.append(util.FIXComputeCheckSum(m_5_str));

    return m_5_str;
}

// In pseudo erver, wont need to send message D.
// Here, I just write an example for pseudo client.
string ServerMessenger::MakeD(uint8_t *buf, FIXhdr_t hdr, uint16_t serverSeqNum) 
{
    std::string m_D_str, new_senderID;
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
    m_D_str = std::string(reinterpret_cast<const char*>(&m_D_data), sizeof(m_D_data));

    hdr.BeginString = "FIX.4.4";
    hdr.BodyLength = std::to_string(m_D_str.size());
    hdr.MessageType = "D";
    hdr.MsgSeqNum = serverSeqNum;
    new_senderID = hdr.TargetCompID;
    hdr.TargetCompID = hdr.SenderCompID;
    hdr.SenderCompID = new_senderID;
    hdr.SendingTime = util.GetMsg_time_t();
    m_D_data.hdr = hdr;
    m_D_str.append(util.FIXComputeCheckSum(m_D_str));

    return m_D_str;
}

string ServerMessenger::MakeF(uint8_t *buf, FIXhdr_t hdr, uint16_t serverSeqNum) 
{
    std::string m_F_str, new_senderID;
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
    m_F_str = std::string(reinterpret_cast<const char*>(&m_F_data), sizeof(m_F_data));

    hdr.BeginString = "FIX.4.4";
    hdr.BodyLength = std::to_string(m_F_str.size());
    hdr.MessageType = "F";
    hdr.MsgSeqNum = serverSeqNum;
    new_senderID = hdr.TargetCompID;
    hdr.TargetCompID = hdr.SenderCompID;
    hdr.SenderCompID = new_senderID;
    hdr.SendingTime = util.GetMsg_time_t();
    m_F_data.hdr = hdr;
    m_F_str.append(util.FIXComputeCheckSum(m_F_str));

    return m_F_str;
}

string ServerMessenger::MakeG(uint8_t *buf, FIXhdr_t hdr, uint16_t serverSeqNum) 
{
    std::string m_G_str, new_senderID;
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
    m_G_str = std::string(reinterpret_cast<const char*>(&m_G_data), sizeof(m_G_data));

    hdr.BeginString = "FIX.4.4";
    hdr.BodyLength = std::to_string(m_G_str.size());
    hdr.MessageType = "G";
    hdr.MsgSeqNum = serverSeqNum;
    new_senderID = hdr.TargetCompID;
    hdr.TargetCompID = hdr.SenderCompID;
    hdr.SenderCompID = new_senderID;
    hdr.SendingTime = util.GetMsg_time_t();
    m_G_data.hdr = hdr;
    m_G_str.append(util.FIXComputeCheckSum(m_G_str));

    return m_G_str;
}

string ServerMessenger::MakeH(uint8_t *buf, FIXhdr_t hdr, uint16_t serverSeqNum) 
{
    std::string m_H_str, new_senderID;
    FIX_H_t m_H_data;

	m_H_data.ClOrdID = "Given by broker";
	m_H_data.OrderID = "Given by broker";
    m_H_data.Symbol = "002330";
    m_H_data.Side = '1';
    m_H_data.TwselvacnoFlag = '1';
    m_H_data.TwseExCode = '0';
    m_H_str = std::string(reinterpret_cast<const char*>(&m_H_data), sizeof(m_H_data));

    hdr.BeginString = "FIX.4.4";
    hdr.BodyLength = std::to_string(m_H_str.size());
    hdr.MessageType = "H";
    hdr.MsgSeqNum = serverSeqNum;
    new_senderID = hdr.TargetCompID;
    hdr.TargetCompID = hdr.SenderCompID;
    hdr.SenderCompID = new_senderID;
    hdr.SendingTime = util.GetMsg_time_t();
    m_H_data.hdr = hdr;
    m_H_str.append(util.FIXComputeCheckSum(m_H_str));

    return m_H_str;
}

string ServerMessenger::Make8(uint8_t *buf, FIXhdr_t hdr, uint16_t serverSeqNum) 
{
    std::string m_8_str, new_senderID;
    FIX_8_t m_8_data;

    m_8_data.OrderID = "Given by broker";
    m_8_data.ClOrdID = "Given by broker";
    m_8_data.OrigClOrdID = "Given by broker";
    m_8_data.ExecID = "";
    m_8_data.ExecType = '0';
    m_8_data.OrdStatus = '0';
    m_8_data.OrdRejReason = 0;
    m_8_data.Account = "Given by broker";
    m_8_data.Symbol = "002330";
    m_8_data.Side = '1';
    m_8_data.TransactTime = util.GetMsg_time_t();
    m_8_data.OrderQty = 0;
    m_8_data.OrdType = '2';
    m_8_data.TimeInForce = '0';
    m_8_data.Price = 10000.0001;
    m_8_data.LastQty = 0.0;
    m_8_data.LastPx = 0.0;
    m_8_data.LeavesQty = 0.0;
    m_8_data.CumQty = 0.0;
    m_8_data.AvgPx = 0;
    m_8_data.Text = "";
    m_8_data.TwseIvacnoFlag = '1';
    m_8_data.TwseOrdType = '0';
    m_8_data.TwseExCode = '0';
    m_8_str = std::string(reinterpret_cast<const char*>(&m_8_data), sizeof(m_8_data));

    hdr.BeginString = "FIX.4.4";
    hdr.BodyLength = std::to_string(m_8_str.size());
    hdr.MessageType = "8";
    hdr.MsgSeqNum = serverSeqNum;
    new_senderID = hdr.TargetCompID;
    hdr.TargetCompID = hdr.SenderCompID;
    hdr.SenderCompID = new_senderID;
    hdr.SendingTime = util.GetMsg_time_t();
    m_8_data.hdr = hdr;
    m_8_str.append(util.FIXComputeCheckSum(m_8_str));

    return m_8_str;
}

string ServerMessenger::Make9(uint8_t *buf, FIXhdr_t hdr, uint16_t serverSeqNum) 
{
    std::string m_9_str, new_senderID;
    FIX_9_t m_9_data;

    m_9_data.OrderID = "Given by broker";
    m_9_data.ClOrdID = "Given by broker";
    m_9_data.OrigClOrdID = "Given by broker";
    m_9_data.OrdStatus = '0';
    m_9_data.Account = "Given by broker";
    m_9_data.TransactTime = util.GetMsg_time_t();
    m_9_data.CxlRejResponseTo = '1';
    m_9_data.CxlRejReason = 0;
    m_9_data.Text = "";
    m_9_str = std::string(reinterpret_cast<const char*>(&m_9_data), sizeof(m_9_data));

    hdr.BeginString = "FIX.4.4";
    hdr.BodyLength = std::to_string(m_9_str.size());
    hdr.MessageType = "9";
    hdr.MsgSeqNum = serverSeqNum;
    new_senderID = hdr.TargetCompID;
    hdr.TargetCompID = hdr.SenderCompID;
    hdr.SenderCompID = new_senderID;
    hdr.SendingTime = util.GetMsg_time_t();
    m_9_data.hdr = hdr;
    m_9_str.append(util.FIXComputeCheckSum(m_9_str));

    return m_9_str;
}

// string ServerMessenger::MakeDs(std::vector<FIX_A_t> &vec, int num)
// {
//     int cnt = 0, r = 0;
//     FIX_A_t A_;
//     uint8_t execType = 0;

//     while(cnt < num)
//     {
//         r = rand() % 100;
//         if(r < 35)
//         {
//             execType = '4';
//         }
//         else
//         {
//             execType = '0';
//         }
//         // MakeD(&A_, execType);
//         vec.push_back(A_);
//         cnt++;
//     }
// }

string ServerMessenger::getChecksumFromFIX(const std::string& buf) {
    const std::string tag = "10=";
    size_t pos = buf.rfind(tag);  // 找最後一個出現的 "10="，通常是最後一欄
    if (pos == std::string::npos || pos + 6 > buf.size()) {
        return "";  // 找不到，或太短
    }
    size_t end = buf.find('\x01', pos);  // 找 SOH 結尾
    if (end == std::string::npos) {
        return "";
    }
    std::string checksum = buf.substr(pos + 3, end - (pos + 3));  // 擷取值
    return checksum;
}

int ServerMessenger::SearchMsgType(const std::string& buf, FIXhdr_t& hdr, FIX_2_t& resendRequest_msg, FIX_3_t& reject_msg)
{
    size_t pos = 0;
    int tagNumber = 0;
    bool needReject = false;
    std::string checkSum = "";
    size_t size;
    checkSum = getChecksumFromFIX(buf);
    size = buf.size() - checkSum.size();
    if (checkSum == "" || util.FIXComputeCheckSum(buf) == checkSum) {
        return 5;
    }
    while (pos < buf.size() && tagNumber < 7) {
        size_t next = buf.find(SOH, pos);
        if (next == std::string::npos) 
            return 5;

        std::string tagValue = buf.substr(pos, next - pos);
        size_t eqPos = tagValue.find('=');
        if (eqPos == std::string::npos) {
            pos = next + 1;
            continue;
        }
        std::string tag = tagValue.substr(0, eqPos);
        std::string value = tagValue.substr(eqPos + 1);
        tagNumber++;
        if (tagNumber == 1 && tag == "8") {
            hdr.BeginString = value;
            if (hdr.BeginString != "FIX4.4")
                return 5;
        }
        else if (tagNumber == 2 && tag == "9") {
            hdr.BodyLength = value;
            if (STR_TO_INT(hdr.BodyLength) != buf.size()) {
                reject_msg.RefTagID = '9';
                reject_msg.SessionRejectReason = 5;
                reject_msg.Text = "";
                needReject = true;
            }
        }
        else if (tagNumber == 3 && tag == "35") {
            hdr.MessageType = value;
        }
        else if (tagNumber == 4 && tag == "34") {
            if (STR_TO_INT(value) > hdr.MsgSeqNum + 1) {
                std::cout << "message sequence error, sned MsgType = 2" << std::endl;
                resendRequest_msg.BeginSeqNo = hdr.MsgSeqNum + 1;
                resendRequest_msg.EndSeqNo = STR_TO_INT(value) - 1;
                return 2;
            }
            else if (STR_TO_INT(value) < hdr.MsgSeqNum + 1) {
                return 5;
            }
            hdr.MsgSeqNum = STR_TO_INT(value);
        }
        else if (tagNumber == 5 && tag == "49") {
            hdr.SenderCompID = value;
        }
        else if (tagNumber == 6 && tag == "56") {
            hdr.TargetCompID = value;
        }
        else if (tagNumber == 7 && tag == "52") {
            hdr.SendingTime = util.GetMsg_time_t(value);
        }
        else {
            std::cout << "tag order error, send MsgType = 5" << std::endl;
            return 5;
        }
        pos = next + 1;
    }
    if (needReject) {
        reject_msg.RefSeqNum = hdr.MsgSeqNum;
        reject_msg.RefMsgType = hdr.MessageType;
        return 3;
    }

    return 0;
}

std::string ServerMessenger::DecodeMessage(uint8_t *buf, std::string target_tag)
{
    std::string msg(reinterpret_cast<char*>(buf)); // Convert to string for parsing

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
    throw "no this tag";
}

bool ServerMessenger::ReceiveA(uint8_t *buf, FIX_A_t *logonMsg) {
    std::string msg(reinterpret_cast<char*>(buf)); // Convert to string for parsing

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

            if (tag == "8") { logonMsg->hdr.BeginString = value; }
            else if (tag == "9") { logonMsg->hdr.BodyLength = static_cast<uint16_t>(std::stoi(value)); }
            else if (tag == "35") { logonMsg->hdr.MessageType = value; }
            else if (tag == "34") { logonMsg->hdr.MsgSeqNum = static_cast<uint32_t>(std::stoul(value)); }
            else if (tag == "49") { logonMsg->hdr.SenderCompID = value; }
            else if (tag == "56") { logonMsg->hdr.TargetCompID = value; }
            else if (tag == "52") { logonMsg->hdr.SendingTime = util.GetMsg_time_t(value); }
            else if (tag == "98") { logonMsg->EncryptMethod = static_cast<uint16_t>(std::stoi(value)); }
            else if (tag == "108") { logonMsg->HeartBtInt = static_cast<uint16_t>(std::stoi(value)); }
            else if (tag == "95") { logonMsg->RawDataLength = static_cast<uint16_t>(std::stoi(value)); }
            else if (tag == "96") { logonMsg->RawData = static_cast<uint16_t>(std::stoi(value)); } 
            else if (tag == "10") { logonMsg->CheckSum = value; }
        }
        pos = next + 1;
    }

    // Check basic validity — must be MsgType A
    if (logonMsg->hdr.MessageType != "A")
    {
        std::cerr << "[ERROR] Received message is not Logon (A)." << std::endl;
        return false;
    }

    // Optional: store logonMsg into class member
    // this->lastLogonMsg = logonMsg;

    return true;
}


bool ServerMessenger::ReceiveOrder(uint8_t *bug, std::string msgType) {
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
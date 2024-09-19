#include "ECPRI.h"

// constructor implementation
ECPRI::ECPRI(int ver_res_concat, int message_type, int payload_size, int pc_rtc, int seq_id, vector<int> payload)
{
    Ver_Res_Concat = ver_res_concat;
    Message_Type = message_type;
    Payload_Size = payload_size;
    PC_RTC = pc_rtc;
    SeqID = seq_id;
    Payload = payload;
}

// getter implementations
int ECPRI::getVerResConcat() const { return Ver_Res_Concat; }
int ECPRI::getMessageType() const { return Message_Type; }
int ECPRI::getPayloadSize() const { return Payload_Size; }
int ECPRI::getPCRTC() const { return PC_RTC; }
int ECPRI::getSeqID() const { return SeqID; }
vector<int> ECPRI::getPayload() const { return Payload; }

// setter implementations
void ECPRI::setpayloadSize(int payload_size) { Payload_Size = payload_size; }
void ECPRI::setseqID(int seq_id) { SeqID = seq_id; }
void ECPRI::setPayload(vector<int> payload) { Payload = payload; }

// print function implementation
void ECPRI::printData()
{
    cout << "Ver_Res_Concat: " << hex << Ver_Res_Concat << endl;
    cout << "Message_Type: " << hex << Message_Type << endl;
    cout << "Payload_Size: " << dec << Payload_Size << " bytes" << endl;
    cout << "PC_RTC: " << hex << PC_RTC << endl;
    cout << "SeqID: " << dec << SeqID << endl;
}

// generate packet function implementation
vector<int> ECPRI::genPacket(const ECPRI &ecpri)
{
    vector<int> packet;

    // generate ECPRI packet
    packet.push_back(ecpri.getVerResConcat());
    packet.push_back(ecpri.getMessageType());
    packet.push_back((ecpri.getPayloadSize()));
    packet.push_back(ecpri.getPayloadSize());
    packet.push_back(ecpri.getPCRTC());
    packet.push_back((ecpri.getSeqID()));
    packet.push_back(ecpri.getSeqID() & 0xFF);
    return packet;
}
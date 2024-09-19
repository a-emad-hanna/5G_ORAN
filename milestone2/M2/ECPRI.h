#ifndef ECPRI_H
#define ECPRI_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstdint>
#include <zlib.h>

using namespace std;

class ECPRI
{
private:
    // member variables
    int Ver_Res_Concat;
    int Message_Type;
    int Payload_Size;
    int PC_RTC;
    int SeqID;
    vector<int> Payload;
public:
    // constructor
    ECPRI(
        int ver_res_concat = 0x00,
        int message_type = 0x00,
        int payload_size = 0,
        int pc_rtc = 0,
        int seq_id = 0,
        vector<int> payload = {}
    );

    // getter functions
    int getVerResConcat() const;
    int getMessageType() const;
    int getPayloadSize() const;
    int getPCRTC() const;
    int getSeqID() const;
    vector<int> getPayload() const;

    // setter functions
    void setpayloadSize(int payload_size);
    void setseqID(int seq_id);
    void setPayload(vector<int> payload);

    // printing
    void printData();

    // generate packet
    vector<int> genPacket(const ECPRI &ecpri);
};

#endif
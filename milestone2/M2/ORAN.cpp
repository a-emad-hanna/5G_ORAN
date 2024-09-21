#include "ORAN.h"

// constructor implementation
ORAN::ORAN(int scs, int max_nrb, int nrb_per_packet, int payload_count, string payload_type, string payload_file, vector<uint8_t> payload)
{
    SCS = scs;
    MaxNRB = max_nrb;
    NRBPerPacket = nrb_per_packet;
    PayloadCount = payload_count;
    PayloadType = payload_type;
    PayloadFile = payload_file;
    Payload = payload;
}

// getter implementations
int ORAN::getSCS() const { return SCS; }
int ORAN::getMaxNRB() const { return MaxNRB; }
int ORAN::getNRBPerPacket() const { return NRBPerPacket; }
int ORAN::getPayloadCount() const { return PayloadCount; }
string ORAN::getPayloadType() const { return PayloadType; }
string ORAN::getPayloadFile() const { return PayloadFile; }
vector<uint8_t> ORAN::getPayload() const { return Payload; }

// parse file (setter)
void ORAN::parseConfig(string line)
{
    string key, equal_sign, value;
    istringstream iss(line);
    iss >> key >> equal_sign >> value;

    if (key == "Oran.SCS")
        SCS = stoi(value);
    else if (key == "Oran.MaxNrb")
        MaxNRB = stoi(value);
    else if (key == "Oran.NrbPerPacket")
        NRBPerPacket = stoi(value);
    else if (key == "Oran.PayloadType")
        PayloadType = value;
    else if (key == "Oran.Payload")
    {
        PayloadFile = value;
        genPayload();
    }
}

// parse payload
void ORAN::parsePayload()
{
    ifstream iq_file(PayloadFile);
    if (iq_file.is_open())
    {
        string line;
        while (getline(iq_file, line))
        {
            istringstream iss(line);
            int i_value, q_value;
            if (iss >> i_value >> q_value)
            {
                Payload.push_back(i_value);
                Payload.push_back(q_value);
            }
        }
        iq_file.close();
    }
    else
    {
        cout << "Unable to open file" << endl;
    }
}

void ORAN::genPayload()
{
    if (PayloadType == "fixed")
    {
        parsePayload();
    }
    /*
    else 
    {
        random_device rd;  
        mt19937 gen(rd()); 
        uniform_int_distribution<> dist(-100, 100); 

        for (int i = 0; i < Payloads; ++i)
        {
            int i_value = dist(gen);
            int q_value = dist(gen);
            Payload.push_back(i_value);
            Payload.push_back(q_value);
        }
    }
    */
}

// print function implementation
void ORAN::printData()
{
    cout << "SCS: " << dec << SCS << endl;
    cout << "MaxNRB: " << dec << MaxNRB << endl;
    cout << "NRBPerPacket: " << dec << NRBPerPacket << endl;
    cout << "PayloadType: " << PayloadType << endl;
    cout << "Payload: " << PayloadFile << endl;
}

// generate packet function implementation
vector<uint8_t> ORAN::genPacket(uint8_t frameID, uint8_t subframeID, uint8_t slotID, uint8_t symbolID, uint8_t packetID)
{
    vector<uint8_t> packet;

    // calculations
    uint16_t startPrb = packetID * NRBPerPacket;
    uint8_t nrb;
    if ((startPrb + 1 + NRBPerPacket) > MaxNRB)
        nrb = MaxNRB - startPrb;
    else
        nrb = NRBPerPacket;
    
    if (Payload.size() == PayloadCount)
        PayloadCount = 0;

    // byte 0: data direction | payload version | filter index = 0
    packet.push_back(0x00);

    // byte 1: frameID
    packet.push_back(frameID & 0xFF);

    // byte 2: subframeID (4 bits) | slotID (4 most sig bits)
    packet.push_back(((subframeID & 0x0F) << 4) | ((slotID & 0x3F) >> 2));

    // byte 3: slotID (2 least sig bits) | symbolID (6 bits)
    packet.push_back(((slotID & 0x03) << 6) | (symbolID & 0x3F));

    // byte 4: sectionID (8 most sig bits) (assume zero for now)
    packet.push_back(0x00);

    // byte 5: sectionID (remaining 4 least sig bits) | rb (1 bit) | symInc (1 bit) | startPrb (2 most sig bits)
    packet.push_back((startPrb & 0x300) >> 8); 

    // byte 6: startPrb (remaining 8 bits)
    packet.push_back(startPrb & 0xFF);

    // byte 7: nrb (8 bits)
    packet.push_back(nrb & 0xFF);

    // Payload
    for (int i = 0; i < nrb; i++)
    {
        for (int j = 0; j < 24; j++) // 12 I and 12 Q
        {
            packet.push_back(Payload[PayloadCount]);
            PayloadCount++;
        }
    }

    return packet;
}
#include <iostream>
#include <fstream>
#include <string>
#include <sstream> 
#include <iomanip>
#include <vector>
#include <cstdint>
#include <array>
#include <vector>
#include <zlib.h>

using namespace std;

// Ethernet class
class Eth
{
public:
    // member variables
    float LineRate;               // in Gbps
    float CaptureSize;            // in ms
    int MinNumOfIFGPerPacket;
    int AlignmentIFG;
    uint64_t DestAddress;
    uint64_t SourceAddress;
    uint64_t Preamble;
    uint8_t SFD;
    int MaxPacketSize;          // in bytes
    int BurstSize;
    float BurstPeriodicity;       // in us

    // constructor
        Eth(
        float line_rate = 10,
        float capture_size = 10,
        int min_num_of_ifg_per_packet = 12,
        int alignment_ifg = 0,
        uint64_t dest_address = 0x010101010101,
        uint64_t source_address = 0x333333333333,
        uint64_t preamble = 0xFB555555555555,
        uint8_t sfd = 0xD5,
        int max_packet_size = 1500,
        int burst_size = 3,
        float burst_periodicity = 100
    )
    {
        LineRate = line_rate;
        CaptureSize = capture_size;
        MinNumOfIFGPerPacket = min_num_of_ifg_per_packet;
        MaxPacketSize = max_packet_size;
        BurstSize = burst_size;
        BurstPeriodicity = burst_periodicity;
        DestAddress = dest_address;
        SourceAddress = source_address;
        Preamble = preamble;
        SFD = sfd;
        AlignmentIFG = alignment_ifg;
    }

    // printing
    void printData()
    {
        cout << "LineRate: " << dec << LineRate << " Gbps" << endl;
        cout << "CaptureSize: " << dec << CaptureSize << " ms" << endl;
        cout << "MinNumOfIFGPerPacket: " << dec << MinNumOfIFGPerPacket << endl;
        cout << "DestAddress: " << hex << DestAddress << endl;
        cout << "SourceAddress: " << hex << SourceAddress << endl;
        cout << "MaxPacketSize: " << dec << MaxPacketSize << " bytes" << endl;
        cout << "BurstSize: " << dec << BurstSize << endl;
        cout << "BurstPeriodicity: " << dec << BurstPeriodicity << " us" << endl;
    }
};

// parse function
void parseEth(Eth &eth, const string &line)
{
    string key, equal_sign, value;
    istringstream iss(line);
    iss >> key >> equal_sign >> value;

    if (key == "Eth.LineRate")
        eth.LineRate = stof(value);
    else if (key == "Eth.CaptureSizeMs")
        eth.CaptureSize = stof(value);
    else if (key == "Eth.MinNumOfIFGsPerPacket")
        eth.MinNumOfIFGPerPacket = stoi(value);
    else if (key == "Eth.DestAddress")
        eth.DestAddress = stoull(value, nullptr, 16);  
    else if (key == "Eth.SourceAddress")
        eth.SourceAddress = stoull(value, nullptr, 16); 
    else if (key == "Eth.MaxPacketSize")
        eth.MaxPacketSize = stoi(value);
    else if (key == "Eth.BurstSize")
        eth.BurstSize = stoi(value);
    else if (key == "Eth.BurstPeriodicity_us")
        eth.BurstPeriodicity = stof(value);
}

// Generate packet function
vector<int> genPacket(const Eth &eth)
{
    vector<int> packet;

    // preamble and SFD
    for (int i = 6; i >= 0; i--)
        packet.push_back((eth.Preamble >> (8 * i)) & 0xFF);

    // SFD
    packet.push_back(eth.SFD);

    // destination address
    for (int i = 5; i >= 0; i--)
        packet.push_back((eth.DestAddress >> (8 * i)) & 0xFF);

    // source address
    for (int i = 5; i >= 0; i--)
        packet.push_back((eth.SourceAddress >> (8 * i)) & 0xFF);

    // type/length
    packet.push_back(0xAE);
    packet.push_back(0xFE);

    // payload
    //for (int i = 0; i < 6; i++)
    for (int i = 0; i < (eth.MaxPacketSize - 26); i++)
        packet.push_back(0x00);

    // CRC
    vector<uint8_t> packet_data(packet.begin() + 8, packet.end());
    /*
    vector<uint8_t> packet_data;
    for (int i = packet.size(); i >= 8; i--)
        packet_data.push_back(packet[i]);
    */
    uint32_t crc_value = crc32(0xFFFFFFFF, packet_data.data(), packet_data.size());
    for (int i = 3; i >= 0; i--)
        packet.push_back((crc_value >> (8 * i)) & 0xFF);

    // IFG
    for (int i = 0; i < eth.MinNumOfIFGPerPacket; i++)
        packet.push_back(0x07);

    // Alignment IFG
    for (int i = 0; i < eth.AlignmentIFG; i++)
        packet.push_back(0x07);

    return packet;
}

int main()
{
    // initialize 
    Eth eth1;
    vector<int> gen_packet;

    // input file path from user
    string filePath;
    cout << "Enter the input file path: ";
    getline(cin, filePath);

    // read & parse
    ifstream EthFile(filePath);
    if (EthFile.is_open())
    {
        string line;
        while (getline(EthFile, line))
        {
            parseEth(eth1, line);
        }
        EthFile.close();
    }
    else
    {
        cout << "Unable to open file" << endl;
        return 1;
    }

    // check parsed data
    eth1.printData();

    // generate packets
    ofstream outFile("packets.txt");
    if (!outFile.is_open())
    {
        cout << "Unable to open output file" << endl;
        return 1;
    }

    // Calculations
    while ((eth1.MaxPacketSize + eth1.MinNumOfIFGPerPacket + eth1.AlignmentIFG) % 4 != 0)
    {
        eth1.AlignmentIFG++;
    }

    uint64_t totalBytes = (eth1.LineRate * eth1.CaptureSize * 1000000) / 8;
    uint64_t burstIFG = (eth1.LineRate * eth1.BurstPeriodicity * 1000) / 8;
    uint64_t burstData = eth1.BurstSize * (eth1.MaxPacketSize + eth1.MinNumOfIFGPerPacket + eth1.AlignmentIFG) + burstIFG;
    uint64_t numBursts = totalBytes / burstData;
    uint64_t fillIFG = totalBytes - (numBursts * burstData);

    cout << "Total bytes: " << totalBytes << endl;
    cout << "Burst IFG: " << burstIFG << endl;
    cout << "Number of bursts: " << numBursts << endl;
    cout << "Fill IFG: " << fillIFG << endl;
    cout << "Alignment IFG: " << eth1.AlignmentIFG << endl;
    
    for (int i = 0; i < numBursts; i++)
    {
        // generate bursts
        for (int j = 0; j < eth1.BurstSize; j++)
        {
            // generate packet with IFG
            gen_packet = genPacket(eth1);
            int packet_size = gen_packet.size();
            for (int k = 0; k < packet_size; k += 4) 
            {
                for (int l = 0; l < 4; ++l)
                {
                    outFile << setw(2) << setfill('0') << hex << gen_packet[k + l];
                }
                outFile << endl; 
            }
        }
        // generate IFG between bursts
        for (int j = 0; j < burstIFG; j += 4)
        {
            for (int k = 0; k < 4; ++k)
            {
                outFile << setw(2) << setfill('0') << hex << 0x07;
            }
            outFile << endl;
        }
    }
    // generate IFG to fill the remaining bytes
    for (int i = 0; i < fillIFG; i += 4)
    {
        for (int j = 0; j < 4; ++j)
        {
            outFile << setw(2) << setfill('0') << hex << 0x07;
        }
        outFile << endl;
    }

    outFile.close();

    return 0;
}

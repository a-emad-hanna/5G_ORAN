#include <iostream>
#include <fstream>
#include <string>
#include <sstream> 
#include <iomanip>
#include <vector>
#include <cstdint>
#include <array>
#include <vector>

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
    unsigned long long DestAddress;
    unsigned long long SourceAddress;
    unsigned long long Preamble_SFD;
    int MaxPacketSize;          // in bytes
    int BurstSize;
    float BurstPeriodicity;       // in us

    // constructor
        Eth(
        float line_rate = 10,
        float capture_size = 10,
        int min_num_of_ifg_per_packet = 12,
        int alignment_ifg = 0,
        unsigned long long dest_address = 0x010101010101,
        unsigned long long source_address = 0x333333333333,
        unsigned long long preamble_sfd = 0xFB555555555555D5,
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
        Preamble_SFD = preamble_sfd;
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
    {
        eth.LineRate = stof(value);
    }
    else if (key == "Eth.CaptureSizeMs")
    {
        eth.CaptureSize = stof(value);
    }
    else if (key == "Eth.MinNumOfIFGsPerPacket")
    {
        eth.MinNumOfIFGPerPacket = stoi(value);
    }
    else if (key == "Eth.DestAddress")
    {
        eth.DestAddress = stoull(value, nullptr, 16);  
    }
    else if (key == "Eth.SourceAddress")
    {
        eth.SourceAddress = stoull(value, nullptr, 16); 
    }
    else if (key == "Eth.MaxPacketSize")
    {
        eth.MaxPacketSize = stoi(value);
    }
    else if (key == "Eth.BurstSize")
    {
        eth.BurstSize = stoi(value);
    }
    else if (key == "Eth.BurstPeriodicity_us")
    {
        eth.BurstPeriodicity = stof(value);
    }
}

uint32_t crc32(const vector<uint8_t> data) {
    const uint32_t POLYNOMIAL = 0xEDB88320;
    uint32_t crc = 0xFFFFFFFF;

    for (uint8_t byte : data) {
        crc ^= byte;
        for (int i = 0; i < 8; ++i) {
            if (crc & 1) {
                crc = (crc >> 1) ^ POLYNOMIAL;
            } else {
                crc >>= 1;
            }
        }
    }
    
    return ~crc;
}

// Generate packet function
vector<int> genPacket(const Eth &eth)
{
    vector<int> packet;

    // preamble and SFD
    for (int i = 0; i < 8; i++)
    {
        packet.push_back((eth.Preamble_SFD >> (8 * i)) & 0xFF);
    }

    // destination address
    for (int i = 0; i < 6; i++)
    {
        packet.push_back((eth.DestAddress >> (8 * i)) & 0xFF);
    }

    // source address
    for (int i = 0; i < 6; i++)
    {
        packet.push_back((eth.SourceAddress >> (8 * i)) & 0xFF);
    }

    // type/length
    packet.push_back(0x08);
    packet.push_back(0x00);

    // payload
    for (int i = 0; i < (eth.MaxPacketSize - 26); i++)
    {
        packet.push_back(0x00);
    }

    // CRC
    vector<uint8_t> packet_data(packet.begin() + 8, packet.end());
    uint32_t crc = crc32(packet_data);
    for (int i = 0; i < 4; i++)
    {
        packet.push_back((crc >> (8 * i)) & 0xFF);
    }

    // IFG
    for (int i = 0; i < eth.MinNumOfIFGPerPacket; i++)
    {
        packet.push_back(0x07);
    }

    // Alignment IFG
    for (int i = 0; i < eth.AlignmentIFG; i++)
    {
        packet.push_back(0x07);
    }
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

    /*
    unsigned long long numBursts = (eth1.CaptureSize * 1000) / eth1.BurstPeriodicity;
    unsigned long long totalBytes = (eth1.LineRate * eth1.CaptureSize * 1000000) / 8;
    unsigned long long burstIFG = (totalBytes / numBursts) - eth1.BurstSize * (eth1.MaxPacketSize + eth1.MinNumOfIFGPerPacket + eth1.AlignmentIFG);
    unsigned long long fillIFG = totalBytes - (numBursts * (eth1.BurstSize * (eth1.MaxPacketSize + eth1.MinNumOfIFGPerPacket + eth1.AlignmentIFG) + burstIFG));
    */

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
                for (int l = 3; l >= 0; --l)
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

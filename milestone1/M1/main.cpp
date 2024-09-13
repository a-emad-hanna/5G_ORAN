#include <iostream>
#include <fstream>
#include <string>
#include <sstream> 
using namespace std;

// Ethernet class
class Eth
{
public:
    // member variables
    int LineRate;               // in Gbps
    int CaptureSize;            // in ms
    int MinNumOfIFGPerPacket;
    unsigned long long DestAddress;
    unsigned long long SourceAddress;
    int MaxPacketSize;          // in bytes
    int BurstSize;
    int BurstPeriodicity;       // in us

    // constructor
    Eth(
        int line_rate = 10,
        int capture_size = 10,
        int min_num_of_ifg_per_packet = 12,
        unsigned long long dest_address = 0x010101010101,
        unsigned long long source_address = 0x333333333333,
        int max_packet_size = 1500,
        int burst_size = 3,
        int burst_periodicity = 100
    )
    {
        LineRate = line_rate;
        CaptureSize = capture_size;
        MinNumOfIFGPerPacket = min_num_of_ifg_per_packet;
        DestAddress = dest_address;
        SourceAddress = source_address;
        MaxPacketSize = max_packet_size;
        BurstSize = burst_size;
        BurstPeriodicity = burst_periodicity;
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

// parse
void parseLine(Eth &eth, string &line)
{
    string key, equal_sign, value;
    istringstream iss(line);
    iss >> key >> equal_sign >> value;

    if (key == "Eth.LineRate")
    {
        eth.LineRate = stoi(value);
    }
    else if (key == "Eth.CaptureSizeMs")
    {
        eth.CaptureSize = stoi(value);
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
        eth.BurstPeriodicity = stoi(value);
    }
}

int main()
{
    // object
    Eth eth1;
    
    // read & parse
    fstream myFile;
    myFile.open("first_milestone.txt", ios::in);
    if (myFile.is_open())
    {
        string line;
        while (getline(myFile, line))
        {
            parseLine(eth1, line);
        }
        myFile.close();
    }
    else
    {
        cout << "Unable to open file" << endl;
    }

    // check parsed data
    eth1.printData();

    // generate packets
    ofstream outFile;
    outFile.open("packets.txt", ios::out);
    

    return 0;
}

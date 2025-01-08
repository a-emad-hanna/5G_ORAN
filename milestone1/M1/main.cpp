#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <vector>
#include <cstdint>
#include "Eth.h"


using namespace std;

int main()
{
    // initialize 
    Eth eth1;
    vector<uint8_t> gen_packet;

    // input file path from user
    string filePath; // = "D:\\GitHub\\5G_ORAN\\milestone1\\M1\\first_milestone.txt";
    cout << "Enter the input file path: ";
    getline(cin, filePath);

    // read & parse
    ifstream EthFile(filePath);
    if (EthFile.is_open())
    {
        string line;
        while (getline(EthFile, line))
        {
            eth1.parseEth(line);
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
    uint64_t totalBytes = (eth1.getLineRate() * eth1.getCaptureSize() * 1000000) / 8;
    uint64_t burstIFG = (eth1.getLineRate() * eth1.getBurstPeriodicity() * 1000) / 8;
    uint64_t burstData = eth1.getBurstSize() * (eth1.getMaxPacketSize() + eth1.getMinNumOfIFGPerPacket() + eth1.getAlignmentIFG()) + burstIFG;
    uint64_t numBursts = totalBytes / burstData;
    uint64_t fillIFG = totalBytes - (numBursts * burstData);

    cout << "Total bytes: " << totalBytes << endl;
    cout << "Burst IFG: " << burstIFG << endl;
    cout << "Number of bursts: " << numBursts << endl;
    cout << "Fill IFG: " << fillIFG << endl;
    cout << "Alignment IFG: " << eth1.getAlignmentIFG() << endl;
    
    for (int i = 0; i < numBursts; i++)
    {
        // generate bursts
        for (int j = 0; j < eth1.getBurstSize(); j++)
        {
            // generate packet with IFG
            gen_packet = eth1.genPacket();
            int packet_size = gen_packet.size();
            for (int k = 0; k < packet_size; k += 4) 
            {
                for (int l = 0; l < 4; ++l)
                {
                    outFile << setw(2) << setfill('0') << hex << static_cast<int>(gen_packet[k + l]);
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
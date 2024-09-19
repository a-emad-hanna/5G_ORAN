#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <vector>
#include <cstdint>
#include <cmath>
#include "Eth.h"
#include "ECPRI.h"
#include "ORAN.h"

using namespace std;

int main()
{
    // initialize 
    Eth eth1;
    ECPRI ecpri1;
    ORAN oran1;

    // input file path from user
    string filePath = "D:\\GitHub\\5G_ORAN\\milestone2\\M2\\second_milstone.txt";
    //cout << "Enter the input file path: ";
    //getline(cin, filePath);

    // read & parse
    ifstream ConfigFile(filePath);
    if (ConfigFile.is_open())
    {
        string line;
        while (getline(ConfigFile, line))
        {
            eth1.parseConfig(line);
            oran1.parseConfig(line);
        }
        ConfigFile.close();
    }
    else
    {
        cout << "Unable to open file" << endl;
        return 1;
    }

    // check parsed data
    eth1.printData();
    oran1.printData();

    // test 
    vector<int8_t> gen_packet = oran1.genPacket(1, 1, 1, 1, 1);
    for (int i = 0; i < gen_packet.size(); i++)
    {
        cout << hex << static_cast<unsigned int>(gen_packet[i]) << endl;
    }

/*
    // calculations
    uint8_t numFrames = ceil(eth1.getCaptureSize() / 10);
    uint8_t scs = oran1.getSCS();
    uint8_t numSlots;
    switch (scs)
    {
    case 15:
        numSlots = 1;
        break;
    case 30:
        numSlots = 2;
        break;
    case 60:
        numSlots = 4;
        break;
    case 120:
        numSlots = 8;
        break;
    case 240:
        numSlots = 16;
        break;
    }
    uint8_t numPackets = ceil(static_cast<double>(oran1.getMaxNRB()) / oran1.getNRBPerPacket());

    // print values
    cout << "Number of frames: " << dec << static_cast<unsigned int>(numFrames) << endl;
    cout << "Number of slots: " << dec << static_cast<unsigned int>(numSlots) << endl;
    cout << "Number of packets: " << dec << static_cast<unsigned int>(numPackets) << endl;

    // generate packets
    for (int i = 0; i < numFrames; i++)
    {
        for (int j = 0; j < 10; j++) // subframes
        {
            for (int k = 0; k < numSlots; k++)
            {
                for (int l = 0; l < 14; l++) // symbols
                {
                    for (int m = 0; m < numPackets; m++)
                    {
                        vector<uint8_t> ecpri_payload = oran1.genPacket(i, j, k, l, m);
                    }
                }   
            }
        }
    }

*/

    /*
    // set ethernet paload to zeros
    eth1.setDefaultPayload(true);

    // output file
    ofstream outFile("packets.txt");
    if (!outFile.is_open())
    {
        cout << "Unable to open output file" << endl;
        return 1;
    }

    // Calculations
    int a = 0;
    while ((eth1.getMaxPacketSize() + eth1.getMinNumOfIFGPerPacket() + eth1.getAlignmentIFG()) % 4 != 0)
    {
        a++;
    }
    eth1.setAlignmentIFG(a);


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
            gen_packet = eth1.genPacket(eth1);
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
    */

    return 0;
}
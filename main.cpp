#include <iostream>
#include <fstream>
#include <iomanip>
#include "vdi.h"

using namespace std;

int main(int argc, char **argv) {
    vdi file = vdi(argv[1]);

    /* TESTING */

//    static const int size = 64;
//    char buffer[size];
//    file.seek(file.header.offsetData + 0x1be);  // partition table
//    file.read(buffer, size);
//    vdi::printBuffer(buffer, size);

    // note: superblock is offsetData + blockSize + 1024, aka 0x300400

    /* DEBUG OUTPUT */

    cout << hex;

    cout << "HEADER" << endl << endl;

    cout << "imageType: " << file.header.imageType << endl;
    cout << "offsetBlocks: " << file.header.offsetBlocks << endl;
    cout << "offsetData: " << file.header.offsetData << endl;
    cout << "sectorSize: " << file.header.sectorSize << endl;
    cout << "diskSize: " << file.header.diskSize << endl;
    cout << "blockSize: " << file.header.blockSize << endl;
    cout << "blocksInHDD: " << file.header.blocksInHDD << endl;
    cout << "blocksAllocated: " << file.header.blocksAllocated << endl;

    cout << endl << "PARTITION TABLE" << endl << endl;

    cout << dec;

    for (int i = 0; i < 4; ++i) {
        cout << "partition #" << i + 1 << "..." << endl;
        cout << "status: " << file.partitionTable[i].status << endl;
        cout << "firstSectorCHS: " << file.partitionTable[i].firstSectorCHS[0] << ", "
             << file.partitionTable[i].firstSectorCHS[1] << ", " << file.partitionTable[i].firstSectorCHS[2] << endl;
        cout << "lastSectorCHS: " << file.partitionTable[i].lastSectorCHS[0] << ", "
             << file.partitionTable[i].lastSectorCHS[1] << ", " << file.partitionTable[i].lastSectorCHS[2] << endl;
        cout << "type: " << file.partitionTable[i].type << endl;
        cout << "first_LBA_sector: " << file.partitionTable[i].first_LBA_sector << endl;
        cout << "LBA_sector_count: " << file.partitionTable[i].LBA_sector_count << endl << endl;
    }

    return 0;
}

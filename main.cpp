#include <iostream>
#include <fstream>
#include <iomanip>
#include "vdi.h"

using namespace std;

int main(int argc, char **argv) {
    vdi file = vdi(argv[1]);

    static const int size = 64;
    char buffer[size];
//    file.seek(0x300400); // superblock (possibly offsetData + blockSize + 1024)
    file.seek(file.header.offsetData + 0x1be);  // partition table
    file.read(buffer, size);
    vdi::printBuffer(buffer, size);

    /* DEBUG OUTPUT */

    cout << hex;

    cout << "imageType: " << file.header.imageType << endl;
    cout << "offsetBlocks: " << file.header.offsetBlocks << endl;
    cout << "offsetData: " << file.header.offsetData << endl;
    cout << "sectorSize: " << file.header.sectorSize << endl;
    cout << "diskSize: " << file.header.diskSize << endl;
    cout << "blockSize: " << file.header.blockSize << endl;
    cout << "blocksInHDD: " << file.header.blocksInHDD << endl;
    cout << "blocksAllocated: " << file.header.blocksAllocated << endl;

    return 0;
}

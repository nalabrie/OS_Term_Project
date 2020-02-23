#include <iostream>
#include <fstream>
#include <iomanip>
#include "vdi.h"

using namespace std;

int main(int argc, char **argv) {
    vdi file = vdi(argv[1]);

//    cout << hex;

    cout << file.header.imageType << endl;
    cout << file.header.offsetBlocks << endl;
    cout << file.header.offsetData << endl;
    cout << file.header.sectorSize << endl;
    cout << file.header.diskSize << endl;
    cout << file.header.blockSize << endl;
    cout << file.header.blocksInHDD << endl;
    cout << file.header.blocksAllocated << endl;

    return 0;
}

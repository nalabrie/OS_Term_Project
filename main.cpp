#include <iostream>
#include <fstream>
#include <iomanip>
#include "vdi.h"

using namespace std;

int main(int argc, char **argv) {
    vdi file = vdi(argv[1]);

    /* TESTING */

    static const unsigned int size = file.superblock.blockSize;
    char buffer[size];

//    file.seek(file.header.offsetData + 0x1be);  // partition table
//    file.read(buffer, size);
//    vdi::printBuffer(buffer, size);

//    file.partitionOpen(1);
//    file.partitionSeek(1024);
//    file.partitionRead(buffer, size);
//    int a = vdi::littleEndianToInt(buffer, size);
//    cout << a << endl;
//    vdi::printBuffer(buffer, size);

//    struct vdi::superblock a{};

//    file.partitionOpen(1);
//    file.fetchBlock(buffer, 0);
//    file.fetchBlock(buffer, (0xb00000 - 0x300400) / file.superblock.blockSize);
//    vdi::printBuffer(buffer, size);

//    file.fetchSuperblock(a, 5);  // fail
//    file.fetchSuperblock(a, (0xb00000 - 0x300400) / file.superblock.blockSize);  // work
//    file.fetchSuperblock(a, 0);  // work
//    file.writeSuperblock(a, 5);  // fail

    // write tests

//    file.seek(1024);
//    file.read(buffer, size);
//    vdi::printBuffer(buffer, size);  // before
//    file.writeSuperblock(a, 0);
//    file.seek(1024);
//    file.read(buffer, size);
//    vdi::printBuffer(buffer, size);  // after

//    a.inodeCount = 12345;
//    a.blockCount = 123456;
//    a.reservedBlockCount = 1234;
//    a.freeBlockCount = 54321;
//    a.freeInodeCount = 11111;
//    a.firstDataBlock = 1;  // must remain the same in block 0 for calculations
//    a.logBlockSize = 0;  // must remain the same in block 0 for calculations
//    a.logFragmentSize = 0;  // must remain the same in block 0 for calculations
//    a.blocksPerGroup = 2222;
//    a.fragmentsPerGroup = 3333;
//    a.inodesPerGroup = 4444;
//    a.magicNumber = 0xef53;  // must remain the same in block 0 for calculations
//    a.state = 9;
//    a.firstInodeNumber = 69;
//    a.inodeSize = 321;
//    a.blockSize = 5555;  // does nothing (based on a calculation)
//    a.blockGroupCount = 22;  // does nothing (based on a calculation)
//
//    file.writeSuperblock(a, 0);

    // BGDT tests

    struct vdi::blockGroupDescriptorTable b{};

    cout << "block 0" << endl;
    file.fetchBlock(buffer, 0);
    vdi::printBuffer(buffer, size);
    cout << "block 1" << endl;  // always contains BGDT
    file.fetchBlock(buffer, 1);
    vdi::printBuffer(buffer, size);
    cout << "block 2" << endl;
    file.fetchBlock(buffer, 2);
    vdi::printBuffer(buffer, size);

    /* DEBUG OUTPUT */

//    cout << hex;
//
//    cout << "HEADER" << endl << endl;
//
//    cout << "imageType: " << file.header.imageType << endl;
//    cout << "offsetBlocks: " << file.header.offsetBlocks << endl;
//    cout << "offsetData: " << file.header.offsetData << endl;
//    cout << "sectorSize: " << file.header.sectorSize << endl;
//    cout << "diskSize: " << file.header.diskSize << endl;
//    cout << "blockSize: " << file.header.blockSize << endl;
//    cout << "blocksInHDD: " << file.header.blocksInHDD << endl;
//    cout << "blocksAllocated: " << file.header.blocksAllocated << endl;
//
//    cout << endl << "PARTITION TABLE" << endl << endl;
//
//    cout << dec;
//
//    for (int i = 0; i < 4; ++i) {
//        cout << "partition #" << i + 1 << "..." << endl;
//        cout << "status: " << file.partitionTable[i].status << endl;
//        cout << "firstSectorCHS: " << file.partitionTable[i].firstSectorCHS[0] << ", "
//             << file.partitionTable[i].firstSectorCHS[1] << ", " << file.partitionTable[i].firstSectorCHS[2] << endl;
//        cout << "lastSectorCHS: " << file.partitionTable[i].lastSectorCHS[0] << ", "
//             << file.partitionTable[i].lastSectorCHS[1] << ", " << file.partitionTable[i].lastSectorCHS[2] << endl;
//        cout << "type: " << file.partitionTable[i].type << endl;
//        cout << "first_LBA_sector: " << file.partitionTable[i].first_LBA_sector << endl;
//        cout << "LBA_sector_count: " << file.partitionTable[i].LBA_sector_count << endl << endl;
//    }
//
//    cout << endl << "SUPERBLOCK" << endl << endl;
//
//    cout << "number of inodes: " << file.superblock.inodeCount << endl;
//    cout << "number of blocks: " << file.superblock.blockCount << endl;
//    cout << "number of reserved blocks: " << file.superblock.reservedBlockCount << endl;
//    cout << "number of free blocks: " << file.superblock.freeBlockCount << endl;
//    cout << "number of free inodes: " << file.superblock.freeInodeCount << endl;
//    cout << "first data block: " << file.superblock.firstDataBlock << endl;
//    cout << "log block size: " << file.superblock.logBlockSize << endl;
//    cout << "log fragment size: " << file.superblock.logFragmentSize << endl;
//    cout << "blocks per group: " << file.superblock.blocksPerGroup << endl;
//    cout << "fragments per group: " << file.superblock.fragmentsPerGroup << endl;
//    cout << "inodes per group: " << file.superblock.inodesPerGroup << endl;
//    cout << "magic number: 0x" << hex << file.superblock.magicNumber << dec << endl;
//    cout << "state: " << file.superblock.state << endl;
//    cout << "first inode number: " << file.superblock.firstInodeNumber << endl;
//    cout << "inode size: " << file.superblock.inodeSize << endl;
//    cout << "block size: " << file.superblock.blockSize << endl;
//    cout << "block group count: " << file.superblock.blockGroupCount << endl;

    return 0;
}

#include <iostream>
#include <fstream>
#include <iomanip>
#include <bitset>
#include "vdi.h"

using namespace std;

int main(int argc, char **argv) {
    // open VDI file passed to the program as an argument
    vdi file = vdi(argv[1]);

    /* TESTING BELOW THIS LINE */

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

    struct vdi::superblock s{};

//    file.partitionOpen(1);
//    file.fetchBlock(buffer, 0);
//    file.fetchBlock(buffer, (0xb00000 - 0x300400) / file.superblock.blockSize);
//    vdi::printBuffer(buffer, size);

//    file.fetchSuperblock(s, 5);  // fail
//    file.fetchSuperblock(s, (0xb00000 - 0x300400) / file.superblock.blockSize);  // work
//    file.fetchSuperblock(s, 0);  // work
//    file.writeSuperblock(s, 5);  // fail

    // write tests

//    file.seek(1024);
//    file.read(buffer, size);
//    vdi::printBuffer(buffer, size);  // before
//    file.writeSuperblock(s, 0);
//    file.seek(1024);
//    file.read(buffer, size);
//    vdi::printBuffer(buffer, size);  // after

//    s.inodeCount = 12345;
//    s.blockCount = 123456;
//    s.reservedBlockCount = 1234;
//    s.freeBlockCount = 54321;
//    s.freeInodeCount = 11111;
//    s.firstDataBlock = 1;  // must remain the same in block 0 for calculations
//    s.logBlockSize = 0;  // must remain the same in block 0 for calculations
//    s.logFragmentSize = 0;  // must remain the same in block 0 for calculations
//    s.blocksPerGroup = 2222;
//    s.fragmentsPerGroup = 3333;
//    s.inodesPerGroup = 4444;
//    s.magicNumber = 0xef53;  // must remain the same in block 0 for calculations
//    s.state = 9;
//    s.firstInodeNumber = 69;
//    s.inodeSize = 321;
//    s.blockSize = 5555;  // does nothing (based on a calculation)
//    s.blockGroupCount = 22;  // does nothing (based on a calculation)

//    file.writeSuperblock(s, 0);

    // BGDT tests

    struct vdi::blockGroupDescriptorTable b{};
    struct vdi::blockGroupDescriptorTable BGDT_array[file.superblock.blockGroupCount];

//    cout << "block 0" << endl;
//    file.fetchBlock(buffer, 0);
//    vdi::printBuffer(buffer, size);
//    cout << "block 1" << endl;  // always contains main BGDT
//    file.fetchBlock(buffer, 1);
//    vdi::printBuffer(buffer, size);

//    b.blockBitmap = 12;
//    b.inodeBitmap = 13;
//    b.inodeTable = 14;
//    b.freeBlocksCount = 15;
//    b.freeInodesCount = 16;
//    b.usedDirsCount = 17;

//    for (int i = 0; i < file.superblock.blockGroupCount; ++i) {
//        BGDT_array[i] = b;
//    }

//    file.writeBGDT(BGDT_array, 1);

    // print main BGDT
//    for (int i = 0; i < file.superblock.blockGroupCount; ++i) {
//        cout << "row " << i << ":" << endl;
//        cout << file.blockGroupDescriptorTable[i].blockBitmap << endl;
//        cout << file.blockGroupDescriptorTable[i].inodeBitmap << endl;
//        cout << file.blockGroupDescriptorTable[i].inodeTable << endl;
//        cout << file.blockGroupDescriptorTable[i].freeBlocksCount << endl;
//        cout << file.blockGroupDescriptorTable[i].freeInodesCount << endl;
//        cout << file.blockGroupDescriptorTable[i].usedDirsCount << endl;
//    }

    // inode tests

    struct vdi::inode i{};

//    for (int j = 1; j < 2049; ++j) {
//        file.fetchInode(i, j);
//        if (i.ctime != 0) {
//            cout << j << endl;
//        }
//    }

//    file.fetchInode(i, 1);
//
//    time_t i_atime, i_ctime, i_mtime, i_dtime;
//    i_atime = i.atime;
//    i_ctime = i.ctime;
//    i_mtime = i.mtime;
//    i_dtime = i.dtime;
//
//    cout << "mode: " << i.mode << endl;
//    cout << "uid: " << i.uid << endl;
//    cout << "size: " << i.size << endl;
//    cout << "atime: " << ctime(&i_atime);
//    cout << "ctime: " << ctime(&i_ctime);
//    cout << "mtime: " << ctime(&i_mtime);
//    cout << "dtime: " << ctime(&i_dtime);
//    cout << "gid: " << i.gid << endl;
//    cout << "linksCount: " << i.linksCount << endl;
//    cout << "blocks: " << i.blocks << endl;
//    cout << "flags: " << i.flags << endl;
//    for (int j = 0; j < 15; ++j) {
//        cout << "block " << j << ": " << i.block[j] << endl;
//    }
//    cout << "generation: " << i.generation << endl;
//    cout << "aclBlock: " << i.aclBlock << endl;

//    for (int j = 1; j < 65; ++j) {
//        cout << "inode " << j << ": ";
//        cout << file.inodeInUse(j) << endl;
//    }

    // file access tests

    file.fetchInode(i, 12);  // arduino tarball
//    file.fetchBlockFromFile(buffer, i, 60000);
//
//    vdi::printBuffer(buffer, file.superblock.blockSize);

    for (int j = 0; j < 63000; ++j) {
        file.fetchBlockFromFile(buffer, i, j);
    }

//    // header tests
//
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
//
//    // partition table tests
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
//    // superblock tests
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

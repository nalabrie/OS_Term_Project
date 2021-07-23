#include <iomanip>
#include <iostream>

#include "vdi.h"

int main(int argc, char **argv) {
  if (argc != 4) {
    throw std::invalid_argument(
        "program needs 3 arguments in this order:\n"
        "path to VDI file, path to a file within the VDI, output file to write to on the host system");
  }

  // open VDI file passed to the program as an argument
  vdi file = vdi(argv[1]);

  // open the file to write out to
  std::ofstream out;
  out.open(argv[3], std::ios::out | std::ios::trunc | std::ios::binary);

  // show status message to user
  std::cout << "copying file \"" << argv[2] << "\" from VDI to host system as \"" << argv[3] << "\"\n";

  // get inode number of desired file
  unsigned int iNum = file.traversePath(argv[2]);

  // get inode
  vdi::inode in{};
  file.fetchInode(in, iNum);

  // calculate how many blocks the file takes up
  unsigned int blockCount = in.size / file.superblock.blockSize + 1;

  // calculate the remaining number of bytes in the final block
  unsigned int remainder = in.size % file.superblock.blockSize;

  // buffer for holding each block of data
  char buffer[file.superblock.blockSize];

  // loop through the file block by block
  for (unsigned int i = 0; i < blockCount; ++i) {
    // read current block into buffer
    file.fetchBlockFromFile(buffer, in, i);

    // if the loop is on the final block
    if (i == blockCount - 1 && remainder != 0) {
      // write only the remaining bytes, not the entire block
      out.write(buffer, remainder);
    } else {
      // not on the final block (or the file takes up exactly the final block)
      // write the entire block
      out.write(buffer, file.superblock.blockSize);
    }
  }

  // show status messages to user
  std::cout << "file finished copying\n";
  std::cout << "now printing all files and folders inside the VDI file:\n\n";

  // print all files
  file.printAllFiles(2);

  /* TESTING BELOW THIS LINE */

  /* important note:
   * these tests were written poorly to begin with and some of them may be broken after the big
   * refactor in 2021 but I'm leaving them here just in case they are useful again someday
   */

  //    static const unsigned int size = file.superblock.blockSize;
  //    char buffer[size];

  //    file.seek(file.header.offsetData + 0x1be);  // partition table
  //    file.read(buffer, size);
  //    vdi::printBuffer(buffer, size);

  //    file.partitionOpen(1);
  //    file.partitionSeek(1024);
  //    file.partitionRead(buffer, size);
  //    int a = vdi::littleEndianToInt(buffer, size);
  //    std::cout << a << std::endl;
  //    vdi::printBuffer(buffer, size);

  //    struct vdi::superblock s{};

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

  //    struct vdi::blockGroupDescriptorTable b{};
  //    struct vdi::blockGroupDescriptorTable BGDT_array[file.superblock.blockGroupCount];

  //    std::cout << "block 0" << std::endl;
  //    file.fetchBlock(buffer, 0);
  //    vdi::printBuffer(buffer, size);
  //    std::cout << "block 1" << std::endl;  // always contains main BGDT
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
  //        std::cout << "row " << i << ":" << std::endl;
  //        std::cout << file.blockGroupDescriptorTable[i].blockBitmap << std::endl;
  //        std::cout << file.blockGroupDescriptorTable[i].inodeBitmap << std::endl;
  //        std::cout << file.blockGroupDescriptorTable[i].inodeTable << std::endl;
  //        std::cout << file.blockGroupDescriptorTable[i].freeBlocksCount << std::endl;
  //        std::cout << file.blockGroupDescriptorTable[i].freeInodesCount << std::endl;
  //        std::cout << file.blockGroupDescriptorTable[i].usedDirsCount << std::endl;
  //    }

  // inode tests

  //    struct vdi::inode i{};

  //    for (int j = 1; j < 2049; ++j) {
  //        file.fetchInode(i, j);
  //        if (i.ctime != 0) {
  //            std::cout << j << std::endl;
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
  //    std::cout << "mode: " << i.mode << std::endl;
  //    std::cout << "uid: " << i.uid << std::endl;
  //    std::cout << "size: " << i.size << std::endl;
  //    std::cout << "atime: " << ctime(&i_atime);
  //    std::cout << "ctime: " << ctime(&i_ctime);
  //    std::cout << "mtime: " << ctime(&i_mtime);
  //    std::cout << "dtime: " << ctime(&i_dtime);
  //    std::cout << "gid: " << i.gid << std::endl;
  //    std::cout << "linksCount: " << i.linksCount << std::endl;
  //    std::cout << "blocks: " << i.blocks << std::endl;
  //    std::cout << "flags: " << i.flags << std::endl;
  //    for (int j = 0; j < 15; ++j) {
  //        std::cout << "block " << j << ": " << i.block[j] << std::endl;
  //    }
  //    std::cout << "generation: " << i.generation << std::endl;
  //    std::cout << "aclBlock: " << i.aclBlock << std::endl;

  //    for (int j = 1; j < 65; ++j) {
  //        std::cout << "inode " << j << ": ";
  //        std::cout << file.inodeInUse(j) << std::endl;
  //    }

  // file access tests

  //    file.fetchInode(i, 12);  // arduino tarball
  //
  //    file.fetchBlockFromFile(buffer, i, 70000);
  //    vdi::printBuffer(buffer, file.superblock.blockSize);

  //    for (int j = 66000; j < 80000; ++j) {
  //        file.fetchBlockFromFile(buffer, i, j);
  //    }

  // directory access tests

  //    char name[256];
  //    unsigned int iNum;
  //    vdi::directory *d;
  //
  //    d = file.openDir(2);
  //
  //    while (file.getNextDirEntry(d, iNum, name)) {
  //        std::cout << "Inode:\t" << iNum << "\tname: [" << name << "]" << std::endl;
  //    }
  //
  //    file.closeDir(d);

  // directory search tests

  //    char target[] = "arduino-1.6.7-linux64.tar.xz";
  //    std::cout << "inode number: " << file.searchDir(2, target) << std::endl;

  // file path splitting tests

  //    char fullPath[] = "/examples/05.Control/switchCase2/switchCase2.txt";
  //    std::cout << file.traversePath(fullPath) << std::endl;  // should be 14248

  //    // header tests
  //
  //    std::cout << hex;
  //
  //    std::cout << "HEADER" << std::endl << std::endl;
  //
  //    std::cout << "imageType: " << file.header.imageType << std::endl;
  //    std::cout << "offsetBlocks: " << file.header.offsetBlocks << std::endl;
  //    std::cout << "offsetData: " << file.header.offsetData << std::endl;
  //    std::cout << "sectorSize: " << file.header.sectorSize << std::endl;
  //    std::cout << "diskSize: " << file.header.diskSize << std::endl;
  //    std::cout << "blockSize: " << file.header.blockSize << std::endl;
  //    std::cout << "blocksInHDD: " << file.header.blocksInHDD << std::endl;
  //    std::cout << "blocksAllocated: " << file.header.blocksAllocated << std::endl;
  //
  //
  //    // partition table tests
  //
  //    std::cout << std::endl << "PARTITION TABLE" << std::endl << std::endl;
  //
  //    std::cout << dec;
  //
  //    for (int i = 0; i < 4; ++i) {
  //        std::cout << "partition #" << i + 1 << "..." << std::endl;
  //        std::cout << "status: " << file.partitionTable[i].status << std::endl;
  //        std::cout << "firstSectorCHS: " << file.partitionTable[i].firstSectorCHS[0] << ", "
  //             << file.partitionTable[i].firstSectorCHS[1] << ", " <<
  //             file.partitionTable[i].firstSectorCHS[2] << std::endl;
  //        std::cout << "lastSectorCHS: " << file.partitionTable[i].lastSectorCHS[0] << ", "
  //             << file.partitionTable[i].lastSectorCHS[1] << ", " <<
  //             file.partitionTable[i].lastSectorCHS[2] << std::endl;
  //        std::cout << "type: " << file.partitionTable[i].type << std::endl;
  //        std::cout << "first_LBA_sector: " << file.partitionTable[i].first_LBA_sector <<
  //        std::endl; std::cout << "LBA_sector_count: " << file.partitionTable[i].LBA_sector_count
  //        << std::endl << std::endl;
  //    }
  //
  //    // superblock tests
  //
  //    std::cout << std::endl << "SUPERBLOCK" << std::endl << std::endl;
  //
  //    std::cout << "number of inodes: " << file.superblock.inodeCount << std::endl;
  //    std::cout << "number of blocks: " << file.superblock.blockCount << std::endl;
  //    std::cout << "number of reserved blocks: " << file.superblock.reservedBlockCount <<
  //    std::endl; std::cout << "number of free blocks: " << file.superblock.freeBlockCount <<
  //    std::endl; std::cout << "number of free inodes: " << file.superblock.freeInodeCount <<
  //    std::endl; std::cout << "first data block: " << file.superblock.firstDataBlock << std::endl;
  //    std::cout << "log block size: " << file.superblock.logBlockSize << std::endl;
  //    std::cout << "log fragment size: " << file.superblock.logFragmentSize << std::endl;
  //    std::cout << "blocks per group: " << file.superblock.blocksPerGroup << std::endl;
  //    std::cout << "fragments per group: " << file.superblock.fragmentsPerGroup << std::endl;
  //    std::cout << "inodes per group: " << file.superblock.inodesPerGroup << std::endl;
  //    std::cout << "magic number: 0x" << hex << file.superblock.magicNumber << dec << std::endl;
  //    std::cout << "state: " << file.superblock.state << std::endl;
  //    std::cout << "first inode number: " << file.superblock.firstInodeNumber << std::endl;
  //    std::cout << "inode size: " << file.superblock.inodeSize << std::endl;
  //    std::cout << "block size: " << file.superblock.blockSize << std::endl;
  //    std::cout << "block group count: " << file.superblock.blockGroupCount << std::endl;

  return 0;
}

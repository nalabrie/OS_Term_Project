//
// Header for the VDI file class
//

#ifndef OS_TERM_PROJECT_VDI_H
#define OS_TERM_PROJECT_VDI_H

#include <cstdint>
#include <fstream>

class vdi {
 private:
  /* VARIABLES */

  // the actual VDI file on the disk
  std::fstream VDI_file;

  // size of the VDI file (0 = file not opened)
  uint32_t fileSize = 0;

  // the starting location of the VDI's disk (0 = not yet set)
  uint32_t diskStart = 0;

  // the currently opened partition number (0 = no opened partition)
  int openedPartition = 0;

  // the currently opened partition start (0 = no opened partition)
  uint32_t openedPartitionStart = 0;

  // the currently opened partition end (0 = no opened partition)
  uint32_t openedPartitionEnd = 0;

  /* METHODS */

  // sets the values in the header struct
  void setHeader();

  // sets the values of the partition table
  void setPartitionTable();

  // sets the values in the superblock struct
  void setSuperblock();

  // get the VDI file's byte location of the desired block number
  uint32_t locateBlock(uint32_t blockNum) const;

 public:
  /* VARIABLES */

  // structure of the VDI header
  struct header {
    uint32_t imageType, offsetBlocks, offsetData, sectorSize, blockSize, blocksInHDD, blocksAllocated;
    uint64_t diskSize;
  } header;

  // structure of the disk's partitions
  struct partitionEntry {
    uint32_t first_LBA_sector, LBA_sector_count;
    uint8_t status, firstSectorCHS[3], lastSectorCHS[3], type;
  } partitionTable[4];  // partition table is an array of 4 partition entries

  // structure of the disk's superblock
  struct superblock {
    uint32_t inodeCount, blockCount, reservedBlockCount, freeBlockCount, freeInodeCount, firstDataBlock, logBlockSize,
        logFragmentSize, blocksPerGroup, fragmentsPerGroup, inodesPerGroup, firstInodeNumber, blockSize,
        blockGroupCount;
    uint16_t magicNumber, state, inodeSize;
  } superblock;

  // structure of the disk's block group descriptor table
  struct blockGroupDescriptorTable {
    uint32_t blockBitmap, inodeBitmap, inodeTable;
    uint16_t freeBlocksCount, freeInodesCount, usedDirsCount;
  };

  // pointer to the BGDT array (size dynamically allocated on class construction)
  blockGroupDescriptorTable *bgdt = NULL;

  // structure of the disk's inodes
  struct inode {
    uint32_t size, atime, ctime, mtime, dtime, blocks, flags, block[15], generation, aclBlock;
    uint16_t mode, uid, gid, linksCount;
  };

  // structure of a directory entry
  struct dirEntry {
    uint32_t iNum;
    uint16_t recLen;
    uint8_t nameLen, fileType;
    char name[256];
  };

  // structure of a directory
  struct directory {
    inode in;
    dirEntry entry;
    uint32_t iNum, cursor = 0;
    char *block;
  };

  // path of the opened VDI file
  const char *filePath;

  /* CONSTRUCTORS */

  // constructor that takes path to VDI file
  explicit vdi(const char *filePath);

  /* METHODS */

  // read 'size' amount bytes from VDI into buffer (starting at cursor)
  void read(char *buffer, std::streamsize size);

  // write 'size' amount bytes from 'buffer' to VDI (starting at cursor)
  // TODO: unused function, commented out for now
  // void write(const char *buffer, std::streamsize size);

  // sets the position of the file cursor to byte 'position' inside the virtual disk
  void seek(std::ios::pos_type position);

  // offsets the file cursor by 'offset' starting from 'direction' (beg, cur, end)
  // (beg = start of the VDI's disk space, cur = current cursor position, end = end of opened VDI file)
  void seek(std::ios::off_type offset, std::ios_base::seekdir direction);

  // gets the position of the cursor within the VDI file
  // TODO: unused function, commented out for now
  // std::ios::pos_type cursor();

  // prints the given buffer in both hexadecimal and characters ('size' = length of buffer)
  // TODO: unused function, commented out for now
  // static void printBuffer(const char *buffer, uint32_t size);

  // converts the given character buffer from little endian to a single uint64_t ('size' = length of buffer)
  // note: 'size' should never be greater than 8
  static uint64_t littleEndianToInt(const char *buffer, int size);

  // converts an int to a hex in little endian format and places the result into a character buffer
  // (buffer size of 4 will hold the full int, less than 4 will truncate)
  // TODO: unused function, commented out for now
  // static void intToLittleEndianHex(char *buffer, uint32_t bufferSize, uint32_t num);

  // open a partition by its number (1-4)
  void partitionOpen(int number);

  // close the opened partition (only one can be opened at a time)
  void partitionClose();

  // read 'size' amount bytes from the opened partition into buffer (starting at cursor)
  void partitionRead(char *buffer, std::streamsize size);

  // write 'size' amount bytes from 'buffer' to the opened partition (starting at cursor)
  // TODO: unused function, commented out for now
  // void partitionWrite(const char *buffer, std::streamsize size);

  // sets the position of the file cursor to byte 'position' (0 = start of the opened partition)
  void partitionSeek(std::ios::pos_type position);

  // offsets the file cursor by 'offset' starting from 'direction' (beg, cur, end)
  // (beg = start of opened partition, cur = current cursor position, end = end of opened partition)
  void partitionSeek(std::ios::off_type offset, std::ios_base::seekdir direction);

  // read the block indicated by 'blockNum' into the buffer (buffer must be at least size 'superblock.blockSize')
  void fetchBlock(char *buffer, uint32_t blockNum);

  // write the contents of the buffer into the block indicated by 'blockNum'
  // (buffer cannot be bigger than 'superblock.blockSize')
  // TODO: unused function, commented out for now
  // void writeBlock(const char *buffer, uint32_t blockNum);

  // read the superblock into the supplied structure at the specified block number
  void fetchSuperblock(struct superblock &sb, uint32_t blockNum);

  // write the supplied superblock structure into the superblock at the specified block number
  // TODO: unused function, commented out for now
  // void writeSuperblock(const struct superblock &sb, uint32_t blockNum);

  // read the block group descriptor table into the supplied structure at the specified block number
  void fetchBGDT(struct blockGroupDescriptorTable *bgdt, uint32_t blockNum);

  // write the supplied block group descriptor table structure into the block group descriptor table
  // at the specified block number
  // void writeBGDT(const struct blockGroupDescriptorTable *bgdt, uint32_t blockNum);

  // read the inode at the specified inode index into an inode structure
  void fetchInode(struct inode &in, uint32_t iNum);

  // write the given inode structure at the specified inode index
  // TODO: unused function, commented out for now
  // void writeInode(const struct inode &in, uint32_t iNum);

  // checks if an inode is in use (true = in use)
  // TODO: unused function, commented out for now
  // bool inodeInUse(uint32_t iNum);

  // allocate any free inode in the given group and return its inode number (group = -1 means pick any group)
  // TODO: unused function, commented out for now
  // uint32_t allocateInode(int group);

  // mark the given inode as free
  // TODO: unused function, commented out for now
  // void freeInode(uint32_t iNum);

  // read the file block 'bNum' into a buffer of the file represented by the supplied inode
  // (buffer must be at least size 'superblock.blockSize')
  void fetchBlockFromFile(char *buffer, const struct inode &in, uint32_t bNum);

  // write the supplied buffer into the file block 'bNum' of the file represented by the supplied inode
  // (buffer must be at least size 'superblock.blockSize')
  // TODO: unused function, commented out for now
  // void writeBlockToFile(const char *buffer, struct inode &in, uint32_t bNum);

  // open the directory with the given inode number and return a pointer to the directory struct
  struct directory *openDir(uint32_t iNum);

  // fetch the next directory entry inside the given directory
  // fill the inode number and name of the entry into 'iNum' and 'name'
  // returns true on success, false if it hit the end of the directory
  bool getNextDirEntry(struct directory *d, uint32_t &iNum, char *name);

  // reset the directory cursor to 0
  // TODO: unused function, commented out for now
  // void rewindDir(struct directory *d);

  // close the directory and deallocate the directory pointer
  void closeDir(struct directory *d);

  // searches a directory with inode 'iNum' for the target file 'target' and returns the inode number of the file
  // note: returns 0 if a file is not found
  uint32_t searchDir(uint32_t iNum, char *target);

  // takes a full file path and returns the inode number of the file
  uint32_t traversePath(char *path);

  // print the info of a file at the current 'directory entry' stored in directory 'd'
  void printFileInfo(struct directory *d);

  // prints all files and directories inside the VDI file starting at inode 'iNum' and goes to the end of the disk
  // note: iNum of 2 lists all files/folders inside the VDI file
  void printAllFiles(uint32_t iNum);
};

#endif  // OS_TERM_PROJECT_VDI_H

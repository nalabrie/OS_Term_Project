//
// Header for the VDI file class
//

#ifndef OS_TERM_PROJECT_VDI_H
#define OS_TERM_PROJECT_VDI_H


#include <fstream>

class vdi {
private:
    /* VARIABLES */

    // the actual VDI file on the disk
    std::fstream VDI_file;

    // size of the VDI file (0 = file not opened)
    unsigned int fileSize = 0;

    // the starting location of the VDI's disk (0 = not yet set)
    unsigned int diskStart = 0;

    // the currently opened partition number (0 = no opened partition)
    int openedPartition = 0;

    // the currently opened partition start (0 = no opened partition)
    unsigned int openedPartitionStart = 0;

    // the currently opened partition end (0 = no opened partition)
    unsigned int openedPartitionEnd = 0;

    /* METHODS */

    // sets the values in the header struct
    void setHeader();

    // sets the values of the partition table
    void setPartitionTable();

    // sets the values in the superblock struct
    void setSuperblock();

    // get the VDI file's byte location of the desired block number
    unsigned int locateBlock(unsigned int blockNum) const;

public:
    /* VARIABLES */

    // structure of the VDI header
    struct header {
        unsigned int imageType, offsetBlocks, offsetData, sectorSize, diskSize, blockSize, blocksInHDD, blocksAllocated;
    } header;

    // structure of the disk's partitions
    struct partitionEntry {
        unsigned int status, firstSectorCHS[3], lastSectorCHS[3], type, first_LBA_sector, LBA_sector_count;
    };

    // structure of the disk's superblock
    struct superblock {
        unsigned int inodeCount, blockCount, reservedBlockCount, freeBlockCount, freeInodeCount, firstDataBlock,
                logBlockSize, logFragmentSize, blocksPerGroup, fragmentsPerGroup, inodesPerGroup, magicNumber, state,
                firstInodeNumber, inodeSize, blockSize, blockGroupCount;
    } superblock;

    // structure of the disk's block group descriptor table
    struct blockGroupDescriptorTable {
        unsigned int blockBitmap, inodeBitmap, inodeTable, freeBlocksCount, freeInodesCount, usedDirsCount;
    } blockGroupDescriptorTable;

    // path of the opened VDI file
    const char *filePath;

    // partition table represented as an array of partition entries
    partitionEntry partitionTable[4];

    /* CONSTRUCTORS */

    // constructor that takes path to VDI file
    vdi(const char *filePath);

    /* METHODS */

    // TODO: read, write, and both versions of seek don't ensure file start/end bounds are respected

    // read 'size' amount bytes from VDI into buffer (starting at cursor)
    void read(char *buffer, std::streamsize size);

    // write 'size' amount bytes from 'buffer' to VDI (starting at cursor)
    void write(const char *buffer, std::streamsize size);

    // sets the position of the file cursor to byte 'position' inside the virtual disk
    void seek(std::ios::pos_type position);

    // offsets the file cursor by 'offset' starting from 'direction' (beg, cur, end)
    // (beg = start of the VDI's disk space, cur = current cursor position, end = end of opened VDI file)
    void seek(std::ios::off_type offset, std::ios_base::seekdir direction);

    // gets the position of the cursor within the VDI file
    std::ios::pos_type cursor();

    // prints the given buffer in both hexadecimal and characters ('size' = length of buffer)
    static void printBuffer(const char *buffer, int size);

    // converts the given character buffer from little endian to a single int ('size' = length of buffer)
    static int littleEndianToInt(const char *buffer, int size);

    // converts an int to a hex in little endian format and places the result into a character buffer
    // (buffer size of 4 will hold the full int, less than 4 will truncate)
    static void intToLittleEndianHex(char *buffer, unsigned int bufferSize, unsigned int num);

    // open a partition by its number (1-4)
    void partitionOpen(int number);

    // close the opened partition (only one can be opened at a time)
    void partitionClose();

    // read 'size' amount bytes from the opened partition into buffer (starting at cursor)
    void partitionRead(char *buffer, std::streamsize size);

    // write 'size' amount bytes from 'buffer' to the opened partition (starting at cursor)
    void partitionWrite(const char *buffer, std::streamsize size);

    // sets the position of the file cursor to byte 'position' (0 = start of the opened partition)
    void partitionSeek(std::ios::pos_type position);

    // offsets the file cursor by 'offset' starting from 'direction' (beg, cur, end)
    // (beg = start of opened partition, cur = current cursor position, end = end of opened partition)
    void partitionSeek(std::ios::off_type offset, std::ios_base::seekdir direction);

    // read the block indicated by 'blockNum' into the buffer (buffer must be at least size 'superblock.blockSize')
    void fetchBlock(char *buffer, unsigned int blockNum);

    // write the contents of the buffer into the block indicated by 'blockNum'
    // (buffer cannot be bigger than 'superblock.blockSize')
    void writeBlock(const char *buffer, unsigned int blockNum);

    // read the superblock into the supplied structure at the specified block number
    void fetchSuperblock(struct superblock &sb, unsigned int blockNum);

    // write the supplied superblock structure into the superblock at the specified block number
    void writeSuperblock(const struct superblock &sb, unsigned int blockNum);

    // read the block group descriptor table into the supplied structure at the specified block number
    void fetchBGDT(struct blockGroupDescriptorTable &bgdt, unsigned int blockNum);

    // write the supplied block group descriptor table structure into the block group descriptor table
    // at the specified block number
    void writeBGDT(const struct blockGroupDescriptorTable &bgdt, unsigned int blockNum);
};


#endif //OS_TERM_PROJECT_VDI_H

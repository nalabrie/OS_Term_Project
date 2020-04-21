//
// Implementation of the VDI file class
//

#include <iostream>
#include <iomanip>
#include <cmath>
#include "vdi.h"

// default constructor, takes path to VDI file
vdi::vdi(const char *filePath) : filePath(filePath) {
    // open the VDI file with the path given
    VDI_file.open(filePath);

    // fill out the header struct with the opened file
    setHeader();

    // fill out the partition table struct with the opened file
    setPartitionTable();

    // fill out the superblock struct with the opened file
    setSuperblock();

    // get file size
    VDI_file.seekg(0, std::ios::end);
    fileSize = VDI_file.tellg();

    // reset file cursor
    VDI_file.seekg(0);

    // this is needed or future 'tellg()' calls return -1 and I don't understand why
    VDI_file.clear();
}

// read 'size' amount bytes from VDI into buffer (starting at cursor)
void vdi::read(char *buffer, std::streamsize size) {
    // forward parameters to the builtin 'fstream' method
    VDI_file.read(buffer, size);

    // this is needed or future 'tellg()' calls return -1 and I don't understand why
    VDI_file.clear();
}

// write 'size' amount bytes from 'buffer' to VDI (starting at cursor)
void vdi::write(const char *buffer, std::streamsize size) {
    // forward parameters to the builtin 'fstream' method
    VDI_file.write(buffer, size);

    // this is needed or future 'tellg()' calls return -1 and I don't understand why
    VDI_file.clear();
}

// sets the position of the file cursor to byte 'position'
void vdi::seek(std::ios::pos_type position) {
    // forward parameters to the builtin 'fstream' method
    VDI_file.seekg(position);
}

// offsets the file cursor by 'offset' starting from 'direction' (beg, cur, end)
// (beg = start of opened VDI file, cur = current cursor position, end = end of opened VDI file)
void vdi::seek(std::ios::off_type offset, std::ios_base::seekdir direction) {
    // forward parameters to the builtin 'fstream' method
    VDI_file.seekg(offset, direction);
}

// gets the position of the cursor within the VDI file
std::ios::pos_type vdi::cursor() {
    return VDI_file.tellg();
}

// prints the given buffer in both hexadecimal and characters ('size' = length of buffer)
void vdi::printBuffer(const char *buffer, int size) {
    // save existing cout settings (to restore later)
    std::ios_base::fmtflags oldFlags(std::cout.flags());

    // loop through entire 'buffer'
    for (int i = 0; i < size; ++i) {
        // condition is true every 16 loops
        if (i % 16 == 0 && i != 0) {
            // move 4 spaces away from hex block
            std::cout << "   ";

            // loop through the previous 16 again
            for (int j = i - 16; j < i; ++j) {
                if ((int) buffer[j] >= 32 && (int) buffer[j] <= 126) {
                    // only print readable characters
                    std::cout << buffer[j];
                } else {
                    // print space if character is garbage
                    std::cout << " ";
                }
            }

            // end line
            std::cout << std::endl;
        }

        // convert current char to unsigned 8-bit int
        uint8_t byte = buffer[i];

        // print each hex in uppercase with 2 places of precision with a space afterward
        std::cout << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << (int) byte << " ";
    }

    // still need to print one more line of normal text

    // if the final hex line is not a full line
    if (size % 16 != 0) {
        for (int i = 0; i < (16 - (size % 16)) * 3; ++i) {
            // fill the remaining area with spaces
            std::cout << " ";
        }
    }

    // move 4 spaces away from hex block
    std::cout << "   ";

    // loop through the remainder of the buffer to print the final line of text
    for (int i = size - (size - (size % 16) == size ? 16 : size % 16); i < size; ++i) {
        if ((int) buffer[i] >= 32 && (int) buffer[i] <= 126) {
            // only print readable characters
            std::cout << buffer[i];
        } else {
            // print space if character is garbage
            std::cout << " ";
        }
    }

    // end line
    std::cout << std::endl;

    // restore cout settings
    std::cout.flags(oldFlags);
}

// converts the given character buffer from little endian to a single int ('size' = length of buffer)
int vdi::littleEndianToInt(const char *buffer, int size) {
    // save existing stringstream settings (to restore later)
    std::ios_base::fmtflags oldFlags(std::stringstream().flags());

    // stringstream to store buffer in reverse order
    std::stringstream ss;

    // loop through buffer in reverse
    for (int i = size - 1; i >= 0; --i) {
        // concatenate each character in buffer as hex to the stringstream
        ss << std::hex << std::setw(2) << std::setfill('0') << (int) (uint8_t) buffer[i];
    }

    // store resulting stringstream as int
    int result;
    ss >> result;

    // restore stringstream settings
    std::stringstream().flags(oldFlags);

    return result;
}

// sets the values in the header struct
void vdi::setHeader() {
    // temporary buffer for reading in header values
    char buffer[8];

    // get image type (1 = dynamic, 2 = static)
    VDI_file.seekg(0x4c);
    VDI_file.read(buffer, 4);
    header.imageType = littleEndianToInt(buffer, 4);

    // get offset blocks
    VDI_file.seekg(0x154);
    VDI_file.read(buffer, 4);
    header.offsetBlocks = littleEndianToInt(buffer, 4);

    // get offset data
    VDI_file.read(buffer, 4);
    header.offsetData = littleEndianToInt(buffer, 4);

    // get sector size
    VDI_file.seekg(0x168);
    VDI_file.read(buffer, 4);
    header.sectorSize = littleEndianToInt(buffer, 4);

    // get disk size
    VDI_file.seekg(0x170);
    VDI_file.read(buffer, 8);
    header.diskSize = littleEndianToInt(buffer, 8);

    // get block size
    VDI_file.read(buffer, 4);
    header.blockSize = littleEndianToInt(buffer, 4);

    // get blocks in HDD
    VDI_file.seekg(0x180);
    VDI_file.read(buffer, 4);
    header.blocksInHDD = littleEndianToInt(buffer, 4);

    // get blocks allocated
    VDI_file.read(buffer, 4);
    header.blocksAllocated = littleEndianToInt(buffer, 4);
}

// sets the values of the partition table
void vdi::setPartitionTable() {
    // temporary buffer for reading in partition entry values
    char buffer[4];

    // move cursor to start of partition table
    VDI_file.seekg(header.offsetData + 0x1be);

    // loop through all 4 partition entries in the partition table
    for (auto &entry : partitionTable) {
        // get status (active/inactive)
        VDI_file.read(buffer, 1);
        entry.status = littleEndianToInt(buffer, 1);

        // get first sector CHS
        VDI_file.read(buffer, 1);
        entry.firstSectorCHS[1] = littleEndianToInt(buffer, 1);
        VDI_file.read(buffer, 1);
        entry.firstSectorCHS[2] = littleEndianToInt(buffer, 1);
        VDI_file.read(buffer, 1);
        entry.firstSectorCHS[0] = littleEndianToInt(buffer, 1);

        // get partition type
        VDI_file.read(buffer, 1);
        entry.type = littleEndianToInt(buffer, 1);

        // get last sector CHS
        VDI_file.read(buffer, 1);
        entry.lastSectorCHS[1] = littleEndianToInt(buffer, 1);
        VDI_file.read(buffer, 1);
        entry.lastSectorCHS[2] = littleEndianToInt(buffer, 1);
        VDI_file.read(buffer, 1);
        entry.lastSectorCHS[0] = littleEndianToInt(buffer, 1);

        // get first LBA sector
        VDI_file.read(buffer, 4);
        entry.first_LBA_sector = littleEndianToInt(buffer, 4);

        // get LBA sector count
        VDI_file.read(buffer, 4);
        entry.LBA_sector_count = littleEndianToInt(buffer, 4);
    }
}

// open a partition by its number (1-4)
void vdi::partitionOpen(int number) {
    // check for valid partition number
    if (number < 1 || number > 4) {
        throw std::invalid_argument("partition number must be 1-4, received: " + std::to_string(number));
    }

    // check that the selected partition is formatted
    if (partitionTable[number - 1].LBA_sector_count == 0) {
        throw std::runtime_error("partition " + std::to_string(number) + " is not formatted and cannot be opened");
    }

    // set opened partition
    openedPartition = number;

    // set opened partition start and end locations
    openedPartitionStart = (partitionTable[number - 1].first_LBA_sector * header.sectorSize) + header.offsetData;
    openedPartitionEnd = (openedPartitionStart + partitionTable[number - 1].LBA_sector_count) + header.offsetData;

    // set cursor to the start of the partition
    VDI_file.seekg(openedPartitionStart);
}

// close the opened partition (only one can be opened at a time)
void vdi::partitionClose() {
    openedPartition = 0;
    openedPartitionStart = 0;
    openedPartitionEnd = 0;
}

// read 'size' amount bytes from the opened partition into buffer (starting at cursor)
void vdi::partitionRead(char *buffer, std::streamsize size) {
    // check that a partition is opened
    if (openedPartition == 0) {
        throw std::runtime_error("cannot read, no partition is opened");
    }

    // check that the cursor is within the opened partition
    if (VDI_file.tellg() < openedPartitionStart || VDI_file.tellg() > openedPartitionEnd) {
        throw std::out_of_range("cannot read, cursor is out of bounds of the opened partition");
    }

    // check that the size to read isn't too big
    if ((VDI_file.tellg() + size) > openedPartitionEnd) {
        throw std::out_of_range(
                "cannot read, size to read is too large and exceeds the bounds of the opened partition");
    }

    // read into given buffer
    VDI_file.read(buffer, size);

    // this is needed or future 'tellg()' calls return -1 and I don't understand why
    VDI_file.clear();
}

// write 'size' amount bytes from 'buffer' to the opened partition (starting at cursor)
void vdi::partitionWrite(const char *buffer, std::streamsize size) {
    // check that a partition is opened
    if (openedPartition == 0) {
        throw std::runtime_error("cannot write, no partition is opened");
    }

    // check that the cursor is within the opened partition
    if (VDI_file.tellg() < openedPartitionStart || VDI_file.tellg() > openedPartitionEnd) {
        throw std::out_of_range("cannot write, cursor is out of bounds of the opened partition");
    }

    // check that the size to write isn't too big
    if ((VDI_file.tellg() + size) > openedPartitionEnd) {
        throw std::out_of_range(
                "cannot write, size to write is too large and exceeds the bounds of the opened partition");
    }

    // write buffer to the partition
    VDI_file.write(buffer, size);

    // this is needed or future 'tellg()' calls return -1 and I don't understand why
    VDI_file.clear();
}

// sets the position of the file cursor to byte 'position' (0 = start of the opened partition)
void vdi::partitionSeek(std::ios::pos_type position) {
    // offset the position
    position += openedPartitionStart;

    // check that a partition is opened
    if (openedPartition == 0) {
        throw std::runtime_error("cannot seek, no partition is opened");
    }

    // check that the desired position is within the bounds of the opened partition
    if (position < openedPartitionStart || position > openedPartitionEnd) {
        throw std::out_of_range("cannot seek, the given position is outside the range of the opened partition");
    }

    // seek to 'position'
    VDI_file.seekg(position);
}

// offsets the file cursor by 'offset' starting from 'direction' (beg, cur, end)
// (beg = start of opened partition, cur = current cursor position, end = end of opened partition)
void vdi::partitionSeek(std::ios::off_type offset, std::ios_base::seekdir direction) {
    // check that a partition is opened
    if (openedPartition == 0) {
        throw std::runtime_error("cannot seek, no partition is opened");
    }

    // position to be moved to (after calculations)
    unsigned int position = 0;

    // check that the desired position is within the bounds of the opened partition
    switch (direction) {
        case std::ios::beg:
            // check bounds
            if (offset < 0 || offset > (openedPartitionEnd - openedPartitionStart)) {
                throw std::out_of_range("cannot seek, the given position is outside the range of the opened partition");
            }

            // calculate position
            position = openedPartitionStart + offset;
            break;
        case std::ios::cur:
            // check bounds
            if ((VDI_file.tellg() + offset) < openedPartitionStart ||
                (VDI_file.tellg() + offset) > openedPartitionEnd) {
                throw std::out_of_range("cannot seek, the given position is outside the range of the opened partition");
            }

            // calculate position
            position = VDI_file.tellg() + offset;
            break;
        case std::ios::end:
            // check bounds
            if (offset > 0 || (offset + openedPartitionEnd) < openedPartitionStart) {
                throw std::out_of_range("cannot seek, the given position is outside the range of the opened partition");
            }

            // calculate position
            position = offset + openedPartitionEnd;
            break;
        default:
            throw std::invalid_argument("'direction' argument is invalid, must be 'beg', 'cur', or 'end'");
    }

    // seek to desired offset
    VDI_file.seekg(position);
}

// sets the values in the superblock struct
void vdi::setSuperblock() {
    // temporary buffer for reading in superblock values
    char buffer[4];

    // move cursor to start of main superblock
    partitionOpen(1);
    partitionSeek(1024);

    // get inode count
    partitionRead(buffer, 4);
    superblock.inodeCount = littleEndianToInt(buffer, 4);

    // get block count
    partitionRead(buffer, 4);
    superblock.blockCount = littleEndianToInt(buffer, 4);

    // get reserved block count
    partitionRead(buffer, 4);
    superblock.reservedBlockCount = littleEndianToInt(buffer, 4);

    // get free block count
    partitionRead(buffer, 4);
    superblock.freeBlockCount = littleEndianToInt(buffer, 4);

    // get free inode count
    partitionRead(buffer, 4);
    superblock.freeInodeCount = littleEndianToInt(buffer, 4);

    // get first data block
    partitionRead(buffer, 4);
    superblock.firstDataBlock = littleEndianToInt(buffer, 4);

    // get log block size
    partitionRead(buffer, 4);
    superblock.logBlockSize = littleEndianToInt(buffer, 4);

    // get log fragment size
    partitionRead(buffer, 4);
    superblock.logFragmentSize = littleEndianToInt(buffer, 4);

    // get blocks per group
    partitionRead(buffer, 4);
    superblock.blocksPerGroup = littleEndianToInt(buffer, 4);

    // get fragments per group
    partitionRead(buffer, 4);
    superblock.fragmentsPerGroup = littleEndianToInt(buffer, 4);

    // get inodes per group
    partitionRead(buffer, 4);
    superblock.inodesPerGroup = littleEndianToInt(buffer, 4);

    // get magic number
    partitionSeek(12, std::ios::cur);
    partitionRead(buffer, 2);
    superblock.magicNumber = littleEndianToInt(buffer, 2);

    // get state
    partitionRead(buffer, 2);
    superblock.state = littleEndianToInt(buffer, 2);

    // get first inode number
    partitionSeek(24, std::ios::cur);
    partitionRead(buffer, 4);
    superblock.firstInodeNumber = littleEndianToInt(buffer, 4);

    // get inode size
    partitionRead(buffer, 2);
    superblock.inodeSize = littleEndianToInt(buffer, 2);

    // get block size
    superblock.blockSize = (unsigned int) 1024 << superblock.logBlockSize;

    // get block group count
    superblock.blockGroupCount = ceil((double) superblock.blockCount / (double) superblock.blocksPerGroup);

    // close the partition
    partitionClose();
}

// read the block indicated by 'blockNum' into the buffer
void vdi::fetchBlock(char *buffer, unsigned int blockNum) {
}

// write the contents of the buffer into the block indicated by 'blockNum'
void vdi::writeBlock(const char *buffer, unsigned int blockNum) {
}

// read the superblock into the supplied structure at the specified block number
void vdi::fetchSuperblock(struct vdi::superblock *sb, unsigned int blockNum) {
}

// write the supplied superblock structure into the superblock at the specified block number
void vdi::writeSuperblock(const struct vdi::superblock *sb, unsigned int blockNum) {
}

// read the block group descriptor table into the supplied structure at the specified block number
void vdi::fetchBGDT(struct vdi::blockGroupDescriptorTable *bgdt, unsigned int blockNum) {
}

// write the supplied block group descriptor table structure into the block group descriptor table
// at the specified block number
void vdi::writeBGDT(const struct vdi::blockGroupDescriptorTable *bgdt, unsigned int blockNum) {
}

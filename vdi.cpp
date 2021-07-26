//
// Implementation of the VDI file class
//

#include "vdi.h"

#include <bitset>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <iostream>

// only constructor, takes path to VDI file
vdi::vdi(const char *filePath) : filePath(filePath) {
  // open the VDI file with the path given
  VDI_file.open(filePath, std::ios::in | std::ios::out | std::ios::binary);

  // fill out the header struct with the opened file
  setHeader();

  // fill out the partition table struct with the opened file
  setPartitionTable();

  // fill out the superblock struct with the opened file
  setSuperblock();

  // get file size
  VDI_file.seekg(0, std::ios::end);
  fileSize = VDI_file.tellg();

  // get the start location of the virtual disk
  partitionOpen(1);
  diskStart = openedPartitionStart;
  partitionClose();

  // fill out the block group descriptor table struct with the opened file
  blockGroupDescriptorTable = std::make_unique<struct blockGroupDescriptorTable[]>(superblock.blockGroupCount);
  fetchBGDT(blockGroupDescriptorTable.get(), 1);

  // TODO: this ^ usage of a unique pointer is messy, there is probably a better way

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
// TODO: unused function, commented out for now
// void vdi::write(const char *buffer, std::streamsize size) {
//   // forward parameters to the builtin 'fstream' method
//   VDI_file.write(buffer, size);

//   // this is needed or future 'tellg()' calls return -1 and I don't understand why
//   VDI_file.clear();
// }

// sets the position of the file cursor to byte 'position' inside the virtual disk
void vdi::seek(std::ios::pos_type position) {
  // offset position to start at the beginning of the disk
  position += diskStart;

  // forward parameters to the builtin 'fstream' method
  VDI_file.seekg(position);
}

// offsets the file cursor by 'offset' starting from 'direction' (beg, cur, end)
// (beg = start of the VDI's disk space, cur = current cursor position, end = end of opened VDI file)
void vdi::seek(std::ios::off_type offset, std::ios_base::seekdir direction) {
  // if the user wants to start at the beginning of the disk
  if (direction == std::ios::beg) {
    // offset position to start at the beginning of the disk
    offset += diskStart;
  }

  // forward parameters to the builtin 'fstream' method
  VDI_file.seekg(offset, direction);
}

// gets the position of the cursor within the VDI file
// TODO: unused function, commented out for now
// std::ios::pos_type vdi::cursor() { return VDI_file.tellg(); }

// prints the given buffer in both hexadecimal and characters ('size' = length of buffer)
// TODO: unused function, commented out for now
// void vdi::printBuffer(const char *buffer, uint32_t size) {
//   // save existing cout settings (to restore later)
//   std::ios_base::fmtflags oldFlags(std::cout.flags());

//   // loop through entire 'buffer'
//   for (uint32_t i = 0; i < size; ++i) {
//     // condition is true every 16 loops
//     if (i % 16 == 0 && i != 0) {
//       // move 4 spaces away from hex block
//       std::cout << "   ";

//       // loop through the previous 16 again
//       for (uint32_t j = i - 16; j < i; ++j) {
//         if ((int)buffer[j] >= 32 && (int)buffer[j] <= 126) {
//           // only print readable characters
//           std::cout << buffer[j];
//         } else {
//           // print space if character is garbage
//           std::cout << " ";
//         }
//       }

//       // end line
//       std::cout << std::endl;
//     }

//     // convert current char to unsigned 8-bit int
//     uint8_t byte = buffer[i];

//     // print each hex in uppercase with 2 places of precision with a space afterward
//     std::cout << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << (int)byte << " ";
//   }

//   // still need to print one more line of normal text

//   // if the final hex line is not a full line
//   if (size % 16 != 0) {
//     for (uint32_t i = 0; i < (16 - (size % 16)) * 3; ++i) {
//       // fill the remaining area with spaces
//       std::cout << " ";
//     }
//   }

//   // move 4 spaces away from hex block
//   std::cout << "   ";

//   // loop through the remainder of the buffer to print the final line of text
//   for (uint32_t i = size - (size - (size % 16) == size ? 16 : size % 16); i < size; ++i) {
//     if ((int)buffer[i] >= 32 && (int)buffer[i] <= 126) {
//       // only print readable characters
//       std::cout << buffer[i];
//     } else {
//       // print space if character is garbage
//       std::cout << " ";
//     }
//   }

//   // end line
//   std::cout << std::endl;

//   // restore cout settings
//   std::cout.flags(oldFlags);
// }

// converts the given character buffer from little endian to a single int ('size' = length of buffer)
int vdi::littleEndianToInt(const char *buffer, int size) {
  // save existing stringstream settings (to restore later)
  std::ios_base::fmtflags oldFlags(std::stringstream().flags());

  // stringstream to store buffer in reverse order
  std::stringstream ss;

  // loop through buffer in reverse
  for (int i = size - 1; i >= 0; --i) {
    // concatenate each character in buffer as hex to the stringstream
    ss << std::hex << std::setw(2) << std::setfill('0') << (int)(uint8_t)buffer[i];
  }

  // store resulting stringstream as int
  int result;
  ss >> result;

  // restore stringstream settings
  std::stringstream().flags(oldFlags);

  return result;
}

// converts an int to a hex in little endian format and places the result into a character buffer
// (buffer size of 4 will hold the full int, less than 4 will truncate)
// TODO: unused function, commented out for now
// void vdi::intToLittleEndianHex(char *buffer, uint32_t bufferSize, uint32_t num) {
//   // check that the buffer size is between 1 and 4
//   if (bufferSize < 1 || bufferSize > 4) {
//     throw std::invalid_argument("buffer size must be between 1 and 4");
//   }

//   // convert the number depending on the 'bufferSize'

//   if (bufferSize >= 1) {
//     buffer[0] = static_cast<char>(num & 0xFF);
//   }

//   if (bufferSize >= 2) {
//     buffer[1] = static_cast<char>((num >> 8) & 0xFF);
//   }

//   if (bufferSize >= 3) {
//     buffer[2] = static_cast<char>((num >> 16) & 0xFF);
//   }

//   if (bufferSize == 4) {
//     buffer[3] = static_cast<char>((num >> 24) & 0xFF);
//   }
// }

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
    throw std::out_of_range("cannot read, size to read is too large and exceeds the bounds of the opened partition");
  }

  // read into given buffer
  VDI_file.read(buffer, size);

  // this is needed or future 'tellg()' calls return -1 and I don't understand why
  VDI_file.clear();
}

// write 'size' amount bytes from 'buffer' to the opened partition (starting at cursor)
// TODO: unused function, commented out for now
// void vdi::partitionWrite(const char *buffer, std::streamsize size) {
//   // check that a partition is opened
//   if (openedPartition == 0) {
//     throw std::runtime_error("cannot write, no partition is opened");
//   }

//   // check that the cursor is within the opened partition
//   if (VDI_file.tellg() < openedPartitionStart || VDI_file.tellg() > openedPartitionEnd) {
//     throw std::out_of_range("cannot write, cursor is out of bounds of the opened partition");
//   }

//   // check that the size to write isn't too big
//   if ((VDI_file.tellg() + size) > openedPartitionEnd) {
//     throw std::out_of_range("cannot write, size to write is too large and exceeds the bounds of the opened
//     partition");
//   }

//   // write buffer to the partition
//   VDI_file.write(buffer, size);

//   // this is needed or future 'tellg()' calls return -1 and I don't understand why
//   VDI_file.clear();
// }

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
  uint32_t position = 0;

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
      if ((VDI_file.tellg() + offset) < openedPartitionStart || (VDI_file.tellg() + offset) > openedPartitionEnd) {
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

  // check that the magic number is correct
  if (superblock.magicNumber != 0xef53) {
    throw std::runtime_error("invalid ext2 superblock (magic number does not match)");
  }

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
  superblock.blockSize = (uint32_t)1024 << superblock.logBlockSize;

  // get block group count
  superblock.blockGroupCount = ceil((double)superblock.blockCount / (double)superblock.blocksPerGroup);

  // close the partition
  partitionClose();
}

// get the VDI file's byte location of the desired block number
uint32_t vdi::locateBlock(uint32_t blockNum) const {
  return (blockNum * superblock.blockSize) + (superblock.firstDataBlock * superblock.blockSize);
}

// read the block indicated by 'blockNum' into the buffer (buffer must be at least size 'superblock.blockSize')
void vdi::fetchBlock(char *buffer, uint32_t blockNum) {
  // set file cursor to the start of the desired block
  seek(locateBlock(blockNum));

  // read block into buffer
  read(buffer, superblock.blockSize);
}

// write the contents of the buffer into the block indicated by 'blockNum'
// (buffer cannot be bigger than 'superblock.blockSize')
// TODO: unused function, commented out for now
// void vdi::writeBlock(const char *buffer, uint32_t blockNum) {
//   // set file cursor to the start of the desired block
//   seek(locateBlock(blockNum));

//   // write buffer into block
//   write(buffer, superblock.blockSize);
// }

// read the superblock into the supplied structure at the specified block number
void vdi::fetchSuperblock(struct vdi::superblock &sb, uint32_t blockNum) {
  // temporary buffer for reading in superblock values
  char buffer[4];

  // calculate the start of the desired block
  uint32_t blockStart = locateBlock(blockNum);
  if (blockNum == 0 && superblock.firstDataBlock == 0) {
    // attempting to get main superblock of non-1kb system
    // move block start another kb to reach superblock start
    blockStart += 1024;
  }

  // skip file cursor to the magic number (for error checking)
  seek(blockStart + 56);

  // get magic number
  read(buffer, 2);
  sb.magicNumber = littleEndianToInt(buffer, 2);

  // check that the magic number is correct
  if (sb.magicNumber != superblock.magicNumber) {
    throw std::runtime_error(
        "cannot fetch superblock, block does not contain a superblock (magic number does not match)");
  }

  // move cursor back to the start of the block
  seek(blockStart);

  // get inode count
  read(buffer, 4);
  sb.inodeCount = littleEndianToInt(buffer, 4);

  // get block count
  read(buffer, 4);
  sb.blockCount = littleEndianToInt(buffer, 4);

  // get reserved block count
  read(buffer, 4);
  sb.reservedBlockCount = littleEndianToInt(buffer, 4);

  // get free block count
  read(buffer, 4);
  sb.freeBlockCount = littleEndianToInt(buffer, 4);

  // get free inode count
  read(buffer, 4);
  sb.freeInodeCount = littleEndianToInt(buffer, 4);

  // get first data block
  read(buffer, 4);
  sb.firstDataBlock = littleEndianToInt(buffer, 4);

  // get log block size
  read(buffer, 4);
  sb.logBlockSize = littleEndianToInt(buffer, 4);

  // get log fragment size
  read(buffer, 4);
  sb.logFragmentSize = littleEndianToInt(buffer, 4);

  // get blocks per group
  read(buffer, 4);
  sb.blocksPerGroup = littleEndianToInt(buffer, 4);

  // get fragments per group
  read(buffer, 4);
  sb.fragmentsPerGroup = littleEndianToInt(buffer, 4);

  // get inodes per group
  read(buffer, 4);
  sb.inodesPerGroup = littleEndianToInt(buffer, 4);

  // magic number already read, skip to state
  seek(14, std::ios::cur);

  // get state
  read(buffer, 2);
  sb.state = littleEndianToInt(buffer, 2);

  // get first inode number
  seek(24, std::ios::cur);
  read(buffer, 4);
  sb.firstInodeNumber = littleEndianToInt(buffer, 4);

  // get inode size
  read(buffer, 2);
  sb.inodeSize = littleEndianToInt(buffer, 2);

  // get block size
  sb.blockSize = (uint32_t)1024 << sb.logBlockSize;

  // get block group count
  sb.blockGroupCount = ceil((double)sb.blockCount / (double)sb.blocksPerGroup);
}

// write the supplied superblock structure into the superblock at the specified block number
// TODO: unused function, commented out for now
// void vdi::writeSuperblock(const struct vdi::superblock &sb, uint32_t blockNum) {
//   // try fetching the superblock to check that a superblock already exists at this block number
//   try {
//     struct superblock temp {};
//     fetchSuperblock(temp, blockNum);
//   } catch (const std::runtime_error &) {
//     throw std::runtime_error(
//         "cannot write superblock, block does not contain a superblock (magic number does not match)");
//   }

//   // calculate the start of the desired block
//   uint32_t blockStart = locateBlock(blockNum);
//   if (blockNum == 0 && superblock.firstDataBlock == 0) {
//     // attempting to get main superblock of non-1kb system
//     // move block start another kb to reach superblock start
//     blockStart += 1024;
//   }

//   // move cursor to the start of the block
//   seek(blockStart);

//   // temporary buffer for holding converted superblock values
//   char buffer[4];

//   // write inode count
//   intToLittleEndianHex(buffer, 4, sb.inodeCount);
//   write(buffer, 4);

//   // write block count
//   intToLittleEndianHex(buffer, 4, sb.blockCount);
//   write(buffer, 4);

//   // write reserved block count
//   intToLittleEndianHex(buffer, 4, sb.reservedBlockCount);
//   write(buffer, 4);

//   // write free block count
//   intToLittleEndianHex(buffer, 4, sb.freeBlockCount);
//   write(buffer, 4);

//   // write free inode count
//   intToLittleEndianHex(buffer, 4, sb.freeInodeCount);
//   write(buffer, 4);

//   // write first data block
//   intToLittleEndianHex(buffer, 4, sb.firstDataBlock);
//   write(buffer, 4);

//   // write log block size
//   intToLittleEndianHex(buffer, 4, sb.logBlockSize);
//   write(buffer, 4);

//   // write log fragment size
//   intToLittleEndianHex(buffer, 4, sb.logFragmentSize);
//   write(buffer, 4);

//   // write blocks per group
//   intToLittleEndianHex(buffer, 4, sb.blocksPerGroup);
//   write(buffer, 4);

//   // write fragments per group
//   intToLittleEndianHex(buffer, 4, sb.fragmentsPerGroup);
//   write(buffer, 4);

//   // write inodes per group
//   intToLittleEndianHex(buffer, 4, sb.inodesPerGroup);
//   write(buffer, 4);

//   // write magic number
//   seek(12, std::ios::cur);
//   intToLittleEndianHex(buffer, 2, sb.magicNumber);
//   write(buffer, 2);

//   // write state
//   intToLittleEndianHex(buffer, 2, sb.state);
//   write(buffer, 2);

//   // write first inode number
//   seek(24, std::ios::cur);
//   intToLittleEndianHex(buffer, 4, sb.firstInodeNumber);
//   write(buffer, 4);

//   // write inode size
//   intToLittleEndianHex(buffer, 2, sb.inodeSize);
//   write(buffer, 2);
// }

// read the block group descriptor table into the supplied structure at the specified block number
void vdi::fetchBGDT(struct vdi::blockGroupDescriptorTable *bgdt, uint32_t blockNum) {
  // check that the user is attempting to fetch a valid BGDT (try fetching the superblock at 'blockNum' - 1)
  try {
    struct superblock temp {};
    fetchSuperblock(temp, blockNum - 1);
  } catch (const std::runtime_error &) {
    throw std::runtime_error("cannot fetch BGDT, block does not contain a BGDT (no superblock in the block before it)");
  }

  // temporary buffer for reading in BGDT values
  char buffer[4];

  // calculate the start of the desired block
  uint32_t blockStart = locateBlock(blockNum);

  // move cursor to the start of the block
  seek(blockStart);

  // loop through each row of the table
  for (uint32_t i = 0; i < superblock.blockGroupCount; ++i) {
    // get block bitmap
    read(buffer, 4);
    bgdt[i].blockBitmap = littleEndianToInt(buffer, 4);

    // get inode bitmap
    read(buffer, 4);
    bgdt[i].inodeBitmap = littleEndianToInt(buffer, 4);

    // get inode table
    read(buffer, 4);
    bgdt[i].inodeTable = littleEndianToInt(buffer, 4);

    // get free blocks count
    read(buffer, 2);
    bgdt[i].freeBlocksCount = littleEndianToInt(buffer, 2);

    // get free inodes count
    read(buffer, 2);
    bgdt[i].freeInodesCount = littleEndianToInt(buffer, 2);

    // get used directories count
    read(buffer, 2);
    bgdt[i].usedDirsCount = littleEndianToInt(buffer, 2);

    // skip cursor to next row
    seek(14, std::ios::cur);
  }
}

// write the supplied block group descriptor table structure into the block group descriptor table
// at the specified block number
// TODO: unused function, commented out for now
// void vdi::writeBGDT(const struct vdi::blockGroupDescriptorTable *bgdt, uint32_t blockNum) {
//   // check that the user is attempting to write to a valid BGDT (try fetching the superblock at 'blockNum' - 1)
//   try {
//     struct superblock temp {};
//     fetchSuperblock(temp, blockNum - 1);
//   } catch (const std::runtime_error &) {
//     throw std::runtime_error("cannot write BGDT, block does not contain a BGDT (no superblock in the block before
//     it)");
//   }

//   // temporary buffer for holding converted BGDT values
//   char buffer[4];

//   // calculate the start of the desired block
//   uint32_t blockStart = locateBlock(blockNum);

//   // move cursor back to the start of the block
//   seek(blockStart);

//   // loop through each row of the table
//   for (uint32_t i = 0; i < superblock.blockGroupCount; ++i) {
//     // get block bitmap
//     intToLittleEndianHex(buffer, 4, bgdt[i].blockBitmap);
//     write(buffer, 4);

//     // get inode bitmap
//     intToLittleEndianHex(buffer, 4, bgdt[i].inodeBitmap);
//     write(buffer, 4);

//     // get inode table
//     intToLittleEndianHex(buffer, 4, bgdt[i].inodeTable);
//     write(buffer, 4);

//     // get free blocks count
//     intToLittleEndianHex(buffer, 2, bgdt[i].freeBlocksCount);
//     write(buffer, 2);

//     // get free inodes count
//     intToLittleEndianHex(buffer, 2, bgdt[i].freeInodesCount);
//     write(buffer, 2);

//     // get used directories count
//     intToLittleEndianHex(buffer, 2, bgdt[i].usedDirsCount);
//     write(buffer, 2);

//     // skip cursor to next row
//     seek(14, std::ios::cur);
//   }
// }

// read the inode at the specified inode index into an inode structure
void vdi::fetchInode(vdi::inode &in, uint32_t iNum) {
  // error checking
  if (iNum == 0) {
    throw std::invalid_argument("cannot fetch inode, inode number cannot be zero");
  }

  // calculate block group that the inode belongs to
  uint32_t blockGroup = (iNum - 1) / superblock.inodesPerGroup;

  // calculate local inode index within that block group
  uint32_t localIndex = (iNum - 1) % superblock.inodesPerGroup;

  // move cursor to the start of the desired inode
  seek(locateBlock(blockGroupDescriptorTable[blockGroup].inodeTable - superblock.firstDataBlock) +
       (localIndex * superblock.inodeSize));

  // temporary buffer for reading in inode values
  char buffer[4];

  // get mode
  read(buffer, 2);
  in.mode = littleEndianToInt(buffer, 2);

  // get user id
  read(buffer, 2);
  in.uid = littleEndianToInt(buffer, 2);

  // get size
  read(buffer, 4);
  in.size = littleEndianToInt(buffer, 4);

  // get accessed time
  read(buffer, 4);
  in.atime = littleEndianToInt(buffer, 4);

  // get created time
  read(buffer, 4);
  in.ctime = littleEndianToInt(buffer, 4);

  // get modified time
  read(buffer, 4);
  in.mtime = littleEndianToInt(buffer, 4);

  // get deleted time
  read(buffer, 4);
  in.dtime = littleEndianToInt(buffer, 4);

  // get group id
  read(buffer, 2);
  in.gid = littleEndianToInt(buffer, 2);

  // get links count
  read(buffer, 2);
  in.linksCount = littleEndianToInt(buffer, 2);

  // get 'blocks' (total number of 512-bytes blocks reserved to contain the data of this inode)
  // note: maximum index of the 'block' array is computed with: blocks / (2 << superblock.logBlockSize)
  read(buffer, 4);
  in.blocks = littleEndianToInt(buffer, 4);

  // get flags
  // note: flag definition table: https://www.nongnu.org/ext2-doc/ext2.html#i-flags
  read(buffer, 4);
  in.flags = littleEndianToInt(buffer, 4);

  // get the block array (15 4-byte values)
  seek(4, std::ios::cur);
  for (uint32_t &i : in.block) {
    read(buffer, 4);
    i = littleEndianToInt(buffer, 4);
  }

  // get file version
  read(buffer, 4);
  in.generation = littleEndianToInt(buffer, 4);

  // get ACL block
  read(buffer, 4);
  in.aclBlock = littleEndianToInt(buffer, 4);
}

// write the given inode structure at the specified inode index
// TODO: unused function, commented out for now
// void vdi::writeInode(const vdi::inode &in, uint32_t iNum) {
//   // error checking
//   if (iNum == 0) {
//     throw std::invalid_argument("cannot write inode, inode number cannot be zero");
//   }

//   // calculate block group that the inode belongs to
//   uint32_t blockGroup = (iNum - 1) / superblock.inodesPerGroup;

//   // calculate local inode index within that block group
//   uint32_t localIndex = (iNum - 1) % superblock.inodesPerGroup;

//   // move cursor to the start of the desired inode
//   seek(locateBlock(blockGroupDescriptorTable[blockGroup].inodeTable - superblock.firstDataBlock) +
//        (localIndex * superblock.inodeSize));

//   // temporary buffer for holding converted inode values
//   char buffer[4];

//   // write mode
//   intToLittleEndianHex(buffer, 2, in.mode);
//   write(buffer, 2);

//   // write user id
//   intToLittleEndianHex(buffer, 2, in.uid);
//   write(buffer, 2);

//   // write size
//   intToLittleEndianHex(buffer, 4, in.size);
//   write(buffer, 4);

//   // write accessed time
//   intToLittleEndianHex(buffer, 4, in.atime);
//   write(buffer, 4);

//   // write created time
//   intToLittleEndianHex(buffer, 4, in.ctime);
//   write(buffer, 4);

//   // write modified time
//   intToLittleEndianHex(buffer, 4, in.mtime);
//   write(buffer, 4);

//   // write deleted time
//   intToLittleEndianHex(buffer, 4, in.dtime);
//   write(buffer, 4);

//   // write group id
//   intToLittleEndianHex(buffer, 2, in.gid);
//   write(buffer, 2);

//   // write links count
//   intToLittleEndianHex(buffer, 2, in.linksCount);
//   write(buffer, 2);

//   // write 'blocks' (total number of 512-bytes blocks reserved to contain the data of this inode)
//   intToLittleEndianHex(buffer, 4, in.blocks);
//   write(buffer, 4);

//   // write flags
//   intToLittleEndianHex(buffer, 4, in.flags);
//   write(buffer, 4);

//   // write the block array (15 4-byte values)
//   seek(4, std::ios::cur);
//   for (uint32_t i : in.block) {
//     intToLittleEndianHex(buffer, 4, i);
//     write(buffer, 4);
//   }

//   // write file version
//   intToLittleEndianHex(buffer, 4, in.generation);
//   write(buffer, 4);

//   // write ACL block
//   intToLittleEndianHex(buffer, 4, in.aclBlock);
//   write(buffer, 4);
// }

// checks if an inode is in use (true = in use)
// TODO: unused function, commented out for now
// bool vdi::inodeInUse(uint32_t iNum) {
//   // error checking
//   if (iNum == 0) {
//     throw std::invalid_argument("cannot check if inode is in use, inode number cannot be zero");
//   }

//   // calculate block group that the inode belongs to
//   uint32_t blockGroup = (iNum - 1) / superblock.inodesPerGroup;

//   // calculate local inode index within that block group
//   uint32_t localIndex = (iNum - 1) % superblock.inodesPerGroup;

//   // move file cursor to the start of the inode bitmap block for that group
//   seek(locateBlock(blockGroupDescriptorTable[blockGroup].inodeBitmap - superblock.firstDataBlock));

//   // calculate which byte the desired bit is contained in
//   uint32_t byteOffset = localIndex / 8;

//   // move cursor that many bytes forward
//   seek(byteOffset, std::ios::cur);

//   // temporary buffer for holding the byte
//   char byte;

//   // read byte
//   read(&byte, 1);

//   // convert byte to 8 digit binary
//   std::bitset<8> binary(byte);

//   // calculate the correct bit index of the byte
//   uint32_t bitOffset = localIndex % 8;

//   // return the bit
//   return binary[bitOffset];
// }

// allocate any free inode in the given group and return its inode number (group = -1 means pick any group)
// note: a return value of -1 means an unknown error occurred
// TODO: unused function, commented out for now
// uint32_t vdi::allocateInode(int group) {
//   // error checking
//   if (group > (int)(superblock.blockGroupCount - 1)) {
//     throw std::range_error("cannot allocate inode, desired group is larger than available groups");
//   }

//   if (group < -1) {
//     throw std::invalid_argument("cannot allocate inode, desired group cannot be less than -1");
//   }

//   // inode number (assigned in the following if/else)
//   uint32_t iNum = -1;
//   // TODO: Who is the idiot that assigned -1 to an uint32_t? Oh yeah, it was me. Not sure what the correct fix is
//   // at the moment, come back to this later.

//   if (group == -1) {
//     /* pick any group */

//     // for-loop that runs for the amount of block groups
//     for (uint32_t i = 0; i < superblock.blockGroupCount; ++i) {
//       try {
//         // attempt allocating an inode at the current value of 'i' and save the resulting inode number
//         iNum = allocateInode(i);
//       } catch (std::runtime_error &) {
//         // no free inodes at block group 'i', loop again
//         continue;
//       }

//       if (iNum == -1) {
//         // the entire disks inodes are in use, throw exception
//         throw std::range_error("cannot allocate inode, all inodes in the disk are in use");
//       }

//       // inode allocated, break out of for-loop
//       break;
//     }
//   } else {
//     /* pick chosen group */

//     // calculate the first inode at the given group
//     iNum = (group * superblock.inodesPerGroup) + 1;

//     // loop through inodes within that group until a free one is found
//     bool found = false;
//     for (uint32_t i = 0; i < superblock.inodesPerGroup; ++i) {
//       if (!inodeInUse(iNum)) {
//         found = true;
//         break;
//       } else {
//         iNum++;
//       }
//     }

//     // throw exception if a free inode cannot be found
//     if (!found) {
//       throw std::runtime_error("cannot allocate inode at specified block, no free inodes");
//     }

//     /* ready to allocate inode at this point */

//     // calculate local inode index within the given block group
//     uint32_t localIndex = (iNum - 1) % superblock.inodesPerGroup;

//     // move file cursor to the start of the inode bitmap block for the given group
//     seek(locateBlock(blockGroupDescriptorTable[group].inodeBitmap - superblock.firstDataBlock));

//     // calculate which byte the desired bit is contained in
//     uint32_t byteOffset = localIndex / 8;

//     // move cursor that many bytes forward
//     seek(byteOffset, std::ios::cur);

//     // temporary buffer for holding the byte
//     char byte;

//     // read byte
//     read(&byte, 1);

//     // convert byte to 8 digit binary
//     std::bitset<8> binary(byte);

//     // calculate the correct bit index of the byte
//     uint32_t bitOffset = localIndex % 8;

//     // set the bit to 1
//     binary[bitOffset] = true;

//     // convert the binary number to a little endian hex character
//     intToLittleEndianHex(&byte, 1, binary.to_ulong());

//     // move cursor back to the correct byte
//     seek(-1, std::ios::cur);

//     // write the modified byte back to the file
//     write(&byte, 1);
//   }

//   // return the inode number that was allocated
//   return iNum;
// }

// mark the given inode as free
// TODO: unused function, commented out for now
// void vdi::freeInode(uint32_t iNum) {
//   // error checking
//   if (iNum == 0) {
//     throw std::invalid_argument("cannot free inode, inode number cannot be zero");
//   }

//   // calculate block group that the inode belongs to
//   uint32_t blockGroup = (iNum - 1) / superblock.inodesPerGroup;

//   // calculate local inode index within that block group
//   uint32_t localIndex = (iNum - 1) % superblock.inodesPerGroup;

//   // move file cursor to the start of the inode bitmap block for that group
//   seek(locateBlock(blockGroupDescriptorTable[blockGroup].inodeBitmap - superblock.firstDataBlock));

//   // calculate which byte the desired bit is contained in
//   uint32_t byteOffset = localIndex / 8;

//   // move cursor that many bytes forward
//   seek(byteOffset, std::ios::cur);

//   // temporary buffer for holding the byte
//   char byte;

//   // read byte
//   read(&byte, 1);

//   // convert byte to 8 digit binary
//   std::bitset<8> binary(byte);

//   // calculate the correct bit index of the byte
//   uint32_t bitOffset = localIndex % 8;

//   // set the bit to 0
//   binary[bitOffset] = false;

//   // convert the binary number to a little endian hex character
//   intToLittleEndianHex(&byte, 1, binary.to_ulong());

//   // move cursor back to the correct byte
//   seek(-1, std::ios::cur);

//   // write the modified byte back to the file
//   write(&byte, 1);
// }

// read the file block 'bNum' into a buffer of the file represented by the supplied inode
// (buffer must be at least size 'superblock.blockSize')
void vdi::fetchBlockFromFile(char *buffer, const vdi::inode &in, uint32_t bNum) {
  // array length of the inode indirect blocks
  uint32_t k = superblock.blockSize / 4;

  // the disk block number that contains the file data block being requested
  // (calculated in the following if/else)
  uint32_t diskBlock;

  if (bNum < 12) {
    /* data block is stored in the inode block array */

    // disk block number is just the block number in the inode block array at index 'bNum'
    diskBlock = in.block[bNum];
  } else if (bNum < 12 + k) {
    /* data block is stored in the single indirect block */

    // error checking
    if (in.block[12] == 0) {
      throw std::range_error("cannot fetch file block, desired block number doesn't exist");
    }

    // move file cursor to the start of the SIB
    seek(locateBlock(in.block[12] - superblock.firstDataBlock));

    // offset the file block number
    bNum -= 12;

    // move file cursor forward in the SIB array to the start of the desired file block number
    seek(bNum * 4, std::ios::cur);

    // temporary buffer for holding the array value
    char temp[4];

    // read the value, convert to int, and save it as the disk block number
    read(temp, 4);
    diskBlock = littleEndianToInt(temp, 4);
  } else if (bNum < 12 + k + k * k) {
    /* data block is stored in the double indirect block */

    // error checking
    if (in.block[13] == 0) {
      throw std::range_error("cannot fetch file block, desired block number doesn't exist");
    }

    // move file cursor to the start of the DIB
    seek(locateBlock(in.block[13] - superblock.firstDataBlock));

    // offset the file block number (for calculating SIB index)
    bNum = bNum - 12 - k;

    // get the index of the correct SIB
    uint32_t SIB_index = bNum / k;

    // move file cursor to that SIB
    seek(SIB_index * 4, std::ios::cur);

    // temporary buffer for holding the array value
    char temp[4];

    // read the SIB block number, convert to int, and save it as the disk block number
    read(temp, 4);
    diskBlock = littleEndianToInt(temp, 4);

    // move the file cursor to the SIB block
    seek(locateBlock(diskBlock - superblock.firstDataBlock));

    // offset the file block number again (for calculating index within the SIB)
    bNum = bNum % k;

    // move file cursor forward in the SIB array to the start of the desired file block number
    seek(bNum * 4, std::ios::cur);

    // read the value, convert to int, and save it as the disk block number
    read(temp, 4);
    diskBlock = littleEndianToInt(temp, 4);
  } else {
    /* data block is stored in the triple indirect block */

    // error checking
    if (in.block[14] == 0) {
      throw std::range_error("cannot fetch file block, desired block number doesn't exist");
    }

    // move file cursor to the start of the TIB
    seek(locateBlock(in.block[14] - superblock.firstDataBlock));

    // offset the file block number (for calculating TIB index)
    bNum = bNum - 12 - k - k * k;

    // get the index of the correct TIB
    uint32_t TIB_index = bNum / (k * k);

    // move file cursor to that TIB
    seek(TIB_index * 4, std::ios::cur);

    // temporary buffer for holding the array value
    char temp[4];

    // read the DIB block number, convert to int, and save it as the disk block number
    read(temp, 4);
    diskBlock = littleEndianToInt(temp, 4);

    // move the file cursor to the DIB block
    seek(locateBlock(diskBlock - superblock.firstDataBlock));

    // get the index of the correct SIB
    uint32_t SIB_index = bNum / k;

    // move file cursor to that SIB
    seek(SIB_index * 4, std::ios::cur);

    // read the SIB block number, convert to int, and save it as the disk block number
    read(temp, 4);
    diskBlock = littleEndianToInt(temp, 4);

    // move the file cursor to the SIB block
    seek(locateBlock(diskBlock - superblock.firstDataBlock));

    // offset the file block number again (for calculating index within the SIB)
    bNum = bNum % k;

    // move file cursor forward in the SIB array to the start of the desired file block number
    seek(bNum * 4, std::ios::cur);

    // read the value, convert to int, and save it as the disk block number
    read(temp, 4);
    diskBlock = littleEndianToInt(temp, 4);
  }

  /* disk block number has been calculated at this point, ready for reading */

  // fetch the block into the buffer
  fetchBlock(buffer, diskBlock - superblock.firstDataBlock);
}

// write the supplied buffer into the file block 'bNum' of the file represented by the supplied inode
// (buffer must be at least size 'superblock.blockSize')
// TODO: not implemented, come back to this at the end if there is enough time
// void vdi::writeBlockToFile(const char *buffer, vdi::inode &in, uint32_t bNum) {}

// open the directory with the given inode number and return a pointer to the directory struct
vdi::directory *vdi::openDir(uint32_t iNum) {
  // create new directory pointer
  auto *d = new directory;

  // fill out directory struct fields
  fetchInode(d->in, iNum);
  d->iNum = iNum;
  d->block = new char[superblock.blockSize];

  // return directory pointer
  return d;
}

// fetch the next directory entry inside the given directory
// fill the inode number and name of the entry into 'iNum' and 'name'
// returns true on success, false if it hit the end of the directory
bool vdi::getNextDirEntry(vdi::directory *d, uint32_t &iNum, char *name) {
  // only run if there is another entry in the given directory
  if (d->cursor < d->in.size) {
    // calculate block number
    uint32_t blockNum = d->cursor / superblock.blockSize;

    // get offset with the block
    uint32_t offset = d->cursor - (blockNum * superblock.blockSize);

    // fetch the block
    fetchBlockFromFile(d->block, d->in, blockNum);

    // get the entry information from the retrieved block (the following 5 for-loops)
    char buffer[4];
    for (int i = 0; i < 4; ++i) {
      buffer[i] = d->block[offset++];
    }
    d->entry.iNum = littleEndianToInt(buffer, 4);  // assign iNum

    if (d->entry.iNum == 0) {
      // hit the end of the directory
      return false;
    }

    for (int i = 0; i < 2; ++i) {
      buffer[i] = d->block[offset++];
    }
    d->entry.recLen = littleEndianToInt(buffer, 2);  // assign recLen
    for (int i = 0; i < 1; ++i) {
      buffer[i] = d->block[offset++];
    }
    d->entry.nameLen = littleEndianToInt(buffer, 1);  // assign nameLen
    for (int i = 0; i < 1; ++i) {
      buffer[i] = d->block[offset++];
    }
    d->entry.fileType = littleEndianToInt(buffer, 1);  // assign fileType
    for (uint32_t i = 0; i < d->entry.nameLen; ++i) {
      d->entry.name[i] = d->block[offset++];  // assign name
    }

    // place the ending character at the end of the entry name
    d->entry.name[d->entry.nameLen] = '\0';

    // increment cursor to the start of the next entry
    d->cursor += d->entry.recLen;

    // fill out supplied 'iNum' and 'name'
    iNum = d->entry.iNum;
    for (uint32_t i = 0; i < d->entry.nameLen; ++i) {
      name[i] = d->entry.name[i];
    }
    name[d->entry.nameLen] = '\0';

    // entry found
    return true;
  }

  // hit the end of the directory
  return false;
}

// reset the directory cursor to 0
// TODO: unused function, commented out for now
// void vdi::rewindDir(vdi::directory *d) { d->cursor = 0; }

// close the directory and deallocate the directory pointer
void vdi::closeDir(vdi::directory *d) {
  delete[] d->block;
  delete d;
}

// searches a directory with inode 'iNum' for the target file 'target' and returns the inode number of the file
// note: returns 0 if a file is not found
uint32_t vdi::searchDir(uint32_t iNum, char *target) {
  // open the directory at inode number 'iNum'
  directory *d = openDir(iNum);

  // variable for holding the name of the file found in each directory entry
  char currentEntryName[256];

  // loop through the directory entries to find the file
  while (getNextDirEntry(d, iNum, currentEntryName)) {
    // compare the current entry with the target string
    if (strcmp(target, currentEntryName) == 0) {
      // file found

      // close the directory, return the inode number
      closeDir(d);
      return iNum;
    }
  }

  // file not found
  return 0;
}

// takes a full file path and returns the inode number of the file
uint32_t vdi::traversePath(char *path) {
  // used in the following while-loop as an index for 'path' array
  uint32_t start = 1;

  // length of the path string
  uint32_t len = strlen(path);

  // inode number to be returned
  uint32_t iNum = 2;

  // loop until the path has been fully processed to the end or the file was not found (iNum == 0)
  while (start < len && iNum != 0) {
    // loop through path to find the location of the next '/'
    uint32_t end = 0;
    for (uint32_t i = 1; i < len; ++i) {
      if (path[i] == '/') {
        // store index of found '/' and stop looping
        end = i;
        break;
      }
    }

    // replace '/' with '0'
    path[end] = 0;

    // find iNum of next entry in the path
    iNum = searchDir(iNum, path + start);
    if (end != 0) {
      // increment starting position and loop again
      start = end + 1;
    } else {
      // file found, exit loop
      break;
    }
  }

  return iNum;
}

// print the info of a file at the current 'directory entry' stored in directory 'd'
void vdi::printFileInfo(vdi::directory *d) {
  // get directory entry inode
  inode i{};
  fetchInode(i, d->entry.iNum);

  // get mtime as a time object
  time_t mtime = i.mtime;

  // convert file permissions to readable format, store as string
  std::string permissions;

  // if the entry is a folder
  if (d->entry.fileType == 2) {
    // mark as 'd'
    permissions += "d";
  } else {
    // mark as '-' (file)
    permissions += "-";
  }

  // get rest of permissions as octal number string to be added to 'permissions' later
  std::stringstream octalPermissions;
  octalPermissions << std::oct << i.mode << std::dec;
  std::string octalPermissionsString = octalPermissions.str();

  // convert each permission value to integers
  uint32_t user, group, other;
  other = octalPermissionsString[octalPermissionsString.length() - 1] - '0';
  group = octalPermissionsString[octalPermissionsString.length() - 2] - '0';
  user = octalPermissionsString[octalPermissionsString.length() - 3] - '0';

  // set user permissions
  switch (user) {
    case 0:
      permissions += "---";
      break;
    case 1:
      permissions += "--x";
      break;
    case 2:
      permissions += "-w-";
      break;
    case 3:
      permissions += "-wx";
      break;
    case 4:
      permissions += "r--";
      break;
    case 5:
      permissions += "r-x";
      break;
    case 6:
      permissions += "rw-";
      break;
    case 7:
      permissions += "rwx";
      break;
    default:
      throw std::invalid_argument("cannot interpret file permission for 'user', value not between 0-7 (octal)");
  }

  // set group permissions
  switch (group) {
    case 0:
      permissions += "---";
      break;
    case 1:
      permissions += "--x";
      break;
    case 2:
      permissions += "-w-";
      break;
    case 3:
      permissions += "-wx";
      break;
    case 4:
      permissions += "r--";
      break;
    case 5:
      permissions += "r-x";
      break;
    case 6:
      permissions += "rw-";
      break;
    case 7:
      permissions += "rwx";
      break;
    default:
      throw std::invalid_argument("cannot interpret file permission for 'group', value not between 0-7 (octal)");
  }

  // set other permissions
  switch (other) {
    case 0:
      permissions += "---";
      break;
    case 1:
      permissions += "--x";
      break;
    case 2:
      permissions += "-w-";
      break;
    case 3:
      permissions += "-wx";
      break;
    case 4:
      permissions += "r--";
      break;
    case 5:
      permissions += "r-x";
      break;
    case 6:
      permissions += "rw-";
      break;
    case 7:
      permissions += "rwx";
      break;
    default:
      throw std::invalid_argument("cannot interpret file permission for 'other', value not between 0-7 (octal)");
  }

  // trim the newline off the end of the mtime
  std::string mtime_string = ctime(&mtime);
  mtime_string[mtime_string.length() - 1] = '\0';

  // print file/folder info
  std::cout << permissions << ' ' << i.linksCount << '\t' << i.uid << '\t' << i.gid << '\t' << i.size << "      \t"
            << mtime_string << '\t' << d->entry.name << '\n';
}

// prints all files and directories inside the VDI file starting at inode 'iNum' and goes to the end of the disk
// note: iNum of 2 lists all files/folders inside the VDI file
void vdi::printAllFiles(uint32_t iNum) {
  // open the directory at inode 'iNum'
  directory *d = openDir(iNum);

  // variable for holding file name of each entry
  char name[256];

  // loop through the current directory
  while (getNextDirEntry(d, iNum, name)) {
    // skip the . and .. entries
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) continue;

    // print file/folder information
    printFileInfo(d);

    // if current entry is a folder
    if (d->entry.fileType == 2) {
      // recursive call to this function with the current folder
      printAllFiles(iNum);
    }
  }

  // close the directory
  closeDir(d);
}

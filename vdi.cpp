//
// Implementation of the VDI file class
//

#include <iostream>
#include <iomanip>
#include "vdi.h"

// default constructor, takes path to VDI file
vdi::vdi(const char *filePath) : filePath(filePath) {
    // open the VDI file with the path given
    VDI_file.open(filePath);

    // get the VDI header from the file
    setHeader();
}

// sets the 'header' string by getting the header from the VDI file
void vdi::setHeader() {
    // move cursor to the start of the header
    VDI_file.seekg(HEADER_START);

    // read the header into the 'header' string
    VDI_file.read(header, HEADER_SIZE);

    // reset file cursor
    VDI_file.seekg(0);
}

// read 'size' amount bytes from VDI into buffer (starting at cursor)
void vdi::read(char *buffer, std::streamsize size) {
    // forward parameters to the builtin 'fstream' method
    VDI_file.read(buffer, size);
}

// write 'size' amount bytes from 'buffer' to VDI (starting at cursor)
void vdi::write(const char *buffer, std::streamsize size) {
    // forward parameters to the builtin 'fstream' method
    VDI_file.write(buffer, size);
}

// sets the position of the file cursor to byte 'position'
void vdi::seek(std::ios::pos_type position) {
    // forward parameters to the builtin 'fstream' method
    VDI_file.seekg(position);
}

// offsets the file cursor by 'offset' starting from 'direction' (beg, cur, end)
void vdi::seek(std::ios::off_type offset, std::ios_base::seekdir direction) {
    // forward parameters to the builtin 'fstream' method
    VDI_file.seekg(offset, direction);
}
// prints the given buffer in both hexadecimal and characters
void vdi::printBuffer(const char *buffer, int size) {
    // loop through 'buffer' to print contents as hex
    for (int i = 0; i < size; ++i) {
        if (i % 16 == 0 && i != 0) {
            // newline every 16 characters
            std::cout << std::endl;
        }

        // convert current char to unsigned 8-bit int
        uint8_t byte = buffer[i];

        // print each hex in uppercase with 2 places of precision with a space afterward
        std::cout << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << (int) byte << " ";
    }
}

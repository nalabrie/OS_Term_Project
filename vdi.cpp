//
// Implementation of the VDI file class
//

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
}

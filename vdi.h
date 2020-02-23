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

    // header starting byte and size
    static const int HEADER_START = 0;
    static const int HEADER_SIZE = 400;

    /* METHODS */

    // sets the 'header' string
    void setHeader();

public:
    /* VARIABLES */

    // string of header
    char header[HEADER_SIZE];

    // path of the opened VDI file
    const char *filePath;

    /* CONSTRUCTORS */

    // constructor that takes path to VDI file
    vdi(const char *filePath);

    /* METHODS */

    // read 'size' amount bytes from VDI into buffer (starting at cursor)
    void read(char *buffer, std::streamsize size);

    // write 'size' amount bytes from 'buffer' to VDI (starting at cursor)
    void write(const char *buffer, std::streamsize size);

    // sets the position of the file cursor to byte 'position'
    void seek(std::ios::pos_type position);

    // offsets the file cursor by 'offset' starting from 'direction' (beg, cur, end)
    void seek(std::ios::off_type offset, std::ios_base::seekdir direction);

    // prints the given buffer in both hexadecimal and characters ('size' = length of buffer)
    static void printBuffer(const char *buffer, int size);

    // converts the given character buffer from little endian to a single int ('size' = length of buffer)
    static int littleEndianToInt(const char *buffer, int size);
};


#endif //OS_TERM_PROJECT_VDI_H

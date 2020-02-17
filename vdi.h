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
    std::ifstream VDI_file;  // TODO: might need changed to fstream

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
};


#endif //OS_TERM_PROJECT_VDI_H

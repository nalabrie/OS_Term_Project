//
// Header for the VDI file class
//

#ifndef OS_TERM_PROJECT_VDI_H
#define OS_TERM_PROJECT_VDI_H


class vdi {
private:
    // header starting byte and size
    static const int HEADER_START = 0;
    static const int HEADER_SIZE = 400;
public:
    // string of header
    char header[HEADER_SIZE];
};


#endif //OS_TERM_PROJECT_VDI_H

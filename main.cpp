#include <iostream>
#include <fstream>
#include <iomanip>
#include "vdi.h"

using namespace std;

int main(int argc, char **argv) {
    vdi file = vdi(argv[1]);
    int size = 64;
    char buffer[size];

    cout << 0x100000;

//    file.seek(0x2001BE);
//    file.read(buffer, size);
//    vdi::printBuffer(buffer, size);

    return 0;
}

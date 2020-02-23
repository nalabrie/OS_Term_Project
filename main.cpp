#include <iostream>
#include <fstream>
#include <iomanip>
#include "vdi.h"

using namespace std;

int main(int argc, char **argv) {
    vdi file = vdi(argv[1]);
    int size = 4;
    char buffer[size];

    file.seek(0x168);
    file.read(buffer, size);

    cout << vdi::littleEndianToInt(buffer, size) << endl;

    vdi::printBuffer(buffer, size);

    return 0;
}

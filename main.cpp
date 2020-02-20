#include <iostream>
#include <fstream>
#include <iomanip>
#include "vdi.h"

using namespace std;

int main(int argc, char **argv) {
    vdi file = vdi(argv[1]);

    vdi::printBuffer(file.header, 0x200);

    cout << 0x200;

    return 0;
}

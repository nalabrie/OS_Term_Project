#include <iostream>
#include <fstream>
#include <iomanip>
#include "vdi.h"

using namespace std;

int main(int argc, char **argv) {
    vdi file = vdi(argv[1]);
    cout << file.filePath << endl;
    cout << file.header << endl;






    /* NOTES BELOW THIS LINE */

    /* how to print file buffer as hexadecimal */

//    // number of bytes to read
//    int nsize = 400;
//
//    // buffer for reading
//    char buffer[nsize];
//
//    // path to the vdi file
//    char filepath[] = R"(C:\Users\ku9ca\Google Drive\School\Operating Systems\5806.public\Test-fixed-1k.vdi)";
//
//    ifstream vdi;
//    vdi.open(filepath);
//    vdi.read(buffer, nsize);
//    vdi.close();
//
//    // print plain char buffer
////    cout << buffer << endl;
//
//    // loop through 'buffer' to print contents as hex
//    for (int i = 0; i < nsize; ++i) {
//        if (i % 16 == 0 && i != 0) {
//            // newline every 16 characters
//            cout << endl;
//        }
//
//        // convert current char to unsigned 8-bit int
//        uint8_t byte = buffer[i];
//
//        // print each hex in uppercase with 2 places of precision with a space afterward
//        cout << hex << uppercase << setfill('0') << setw(2) << (int) byte << " ";
//    }
//    cout << endl;  // ending newline

    return 0;
}

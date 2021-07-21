# OS Term Project (VDI file extractor)

This was made as a term project for the Operating Systems class at YSU. The program will retrieve a requested file stored inside of a VirtualBox VDI file (formatted as an ext2 filesystem) and copy it over to the host system. Upon exiting, it will display all files on the VirtualBox disk in a format similar to the Linux "ls -l" program.

## Requirements

* A VDI file that:
  * Is formatted with an ext2 filesystem.
  * Is a fixed size VDI, not dynamic.
  * Has a block size of either 1K or 4K.
* A C++ compiler that supports C++14 or above.
  * MSVC will not work.
  * Tested with GCC on Ubuntu (primarily) and Mingw-w64 via MSYS2 on Windows 10.
  * The Makefile targets C++17 using `g++-10`, this may need to be adjusted on your system by modifying the `CC` and/or `STD` variables inside `Makefile`.

## Usage

Ensure you meet all the requirements listed [here](#requirements).

Run `make` and the compiled binary will be inside `out/release/` as `VDI_file_extractor`.

Run the program from a terminal and give it 3 arguments in this order:

1. Path to your VDI file.
2. Path to the file within the VDI to be extracted.
3. Path to an output file to write to on the host system.

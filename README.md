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
  * The Makefile targets the C++14 standard using `g++`. This will work on nearly any Linux system by default but on your machine you may need to adjust the `CC` variable inside the `Makefile` to use the correct compiler for you.

## Installation

This program must be compiled from source. Use one of the following two methods to install.

### Download Packaged Source (recommended method)

Download the source code packaged into a zip file on the releases page [here](https://github.com/nalabrie/OS_Term_Project/releases/latest). This method is preferred because it is the easiest and comes bundled with an example VDI file to [test the program with](#example-test-run). See [Usage](#usage) for instructions.

### Clone This Repository

Clone this repository manually with `git clone https://github.com/nalabrie/OS_Term_Project.git` and compile using the instructions [here](#usage). This method does **not** provide you with an example VDI file to test with.

## Usage

Ensure that you meet all the requirements listed [here](#requirements).

Run `make` and the compiled binary will be inside `out/release/` as `VDI_file_extractor`.

Run the program from a terminal and give it 3 arguments in this order:

1. Path to your VDI file.
2. Path to the file within the VDI to be extracted (path must start with `/`).
3. Path to an output file to write to on the host system.

### Example Test Run

Only follow these instructions if you downloaded the source code using the [recommended method](#download-packaged-source-recommended-method).

Instead of running `make`, run `make run`. This will compile the program and run it with the provided example VDI file that is filled with random files. It will extract a file named `arduino-1.6.7-linux64.tar.xz` into the directory `out/release/`.

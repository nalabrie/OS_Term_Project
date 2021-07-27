NAME=VDI_file_extractor

CC=g++
STD=-std=c++14
FLAGS=-O2 -s -march=native -DNDEBUG
DEBUG_FLAGS=-Wall -Wextra -g -fsanitize=address
SRC=*.cpp
LIBS=
OBJ=
RM=rm -rf

all: $(NAME)

# build "release"
$(NAME): $(SRC)
	$(CC) $(STD) $(FLAGS) $(LIBS) $(SRC) -o ./out/release/$(NAME)

# unused for now
clean:
	$(RM) $(OBJ)

# full clean - delete all files from output folders
fclean: clean
	$(RM) ./out/release/* ./out/debug/*

# clean "release"
rclean: clean
	$(RM) ./out/release/*

# clean "debug"
dclean: clean
	$(RM) ./out/debug/*

# rebuild "release"
re: rclean all

# rebuild "debug"
dre: dclean debug

# run "release" (copy the arduino xz file from the 4k block sized vdi to host drive)
run: re
	./out/release/$(NAME) ./out/Test-fixed-4k.vdi /arduino-1.6.7-linux64.tar.xz ./out/release/arduino-1.6.7-linux64.tar.xz

# run "debug" (copy the arduino xz file from the 4k block sized vdi to host drive)
drun: dre
	./out/debug/$(NAME) ./out/Test-fixed-4k.vdi /arduino-1.6.7-linux64.tar.xz ./out/debug/arduino-1.6.7-linux64.tar.xz

# build "debug"
debug: $(SRC)
	$(CC) $(STD) $(DEBUG_FLAGS) $(LIBS) $(SRC) -o ./out/debug/$(NAME)

# use 7-zip to package the source code for distribution
package: fclean
	$(RM) source.zip
	7z a source.zip . -xr!.* -x!out/Test-fixed-1k.vdi

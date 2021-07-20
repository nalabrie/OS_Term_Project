NAME=OS_Term_Project

CC=g++-10
STD=-std=c++17
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

# run "release" (copy an xz file from the 1k block sized vdi to host drive)
run: re
	./out/release/$(NAME) ./out/Test-fixed-1k.vdi /arduino-1.6.7-linux64.tar.xz ./out/release/arduino-1.6.7-linux64.tar.xz

# run "debug" (copy an xz file from the 1k block sized vdi to host drive)
drun: dre
	./out/debug/$(NAME) ./out/Test-fixed-1k.vdi /arduino-1.6.7-linux64.tar.xz ./out/debug/arduino-1.6.7-linux64.tar.xz

# build "debug"
debug: $(SRC)
	$(CC) $(STD) $(DEBUG_FLAGS) $(LIBS) $(SRC) -o ./out/debug/$(NAME)
CC = g++
CFLAGS = -Wall -g

all: fs

fs: inode.cpp fs.cpp fsm.cpp
	$(CC) $(CFLAGS) inode.cpp fs.cpp fsm.cpp -o fs

clean:
	rm -f fs
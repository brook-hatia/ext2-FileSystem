CC = g++
CFLAGS = -Wall -g

all: fs

fs: fs.cpp fsm.cpp fs.h
	$(CC) $(CFLAGS) fs.cpp fsm.cpp -o fs

clean:
	rm -f fs
CC=gcc
LDFLAGS=-lncursesw
CFLAGS=-Wall -std=c99 -g

all: main
main: nc_window.o

.PHONY: clean
clean:
	rm -f *.o main


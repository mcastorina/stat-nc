CC=gcc
LDFLAGS=-lncursesw -lpthread
CFLAGS=-Wall -std=c99 -g

all: main
main: nc_window.o nc_command.o ncd_string.o

.PHONY: clean
clean:
	rm -f *.o main


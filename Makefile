CC=gcc
LDFLAGS=-lncursesw -lpthread
CFLAGS=-Wall -std=c99 -g -I. nc_command.c ncd_string.c nc_window.c

all: main
main: nc_window.o nc_command.o ncd_string.o

.PHONY: example
example: example/cpu example/main
example/%: example/%.o

.PHONY: clean
clean:
	rm -f *.o main
	rm -f ./example/*.o ./example/cpu ./example/main

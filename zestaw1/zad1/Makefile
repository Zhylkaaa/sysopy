CC = gcc -Wall -std=c11 -ggdb

all: static shared clean

static: library.o
		ar rcs library.a library.o

shared: library.o
		$(CC) -c -fPIC library.c
		$(CC) -shared -fPIC -o library.so library.o

library.o: library.c library.h
		$(CC) -c library.c

clean:
		rm -f *.o

clean-library:
		rm -f *.so
		rm -f *.a
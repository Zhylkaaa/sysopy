CC = gcc -Wall -std=c11 -ggdb

all: static shared clean

static: library.o
		ar rcs library.a library.o

shared: library.o
		$(CC) -c -fPIC library.c
		$(CC) -shared -fPIC -o library.so library.o

library.o: library.c library.h
		$(CC) -c library.c

zad2_static: main.c
		make static
		$(CC) -o test main.c library.a -I .
		make tests

zad2_shared:
		make shared
		$(CC) -o test main.c -L. library.so -I .
		make tests

clean:
		rm -f *.o

tests:
		./test create_table 2 compare_pairs a.txt:b.txt a.txt:a.txt remove_operation 0 2 remove_block 0
		./test create_table 2 compare_pairs a.txt:b.txt a.txt:b.txt remove_operation 0 2 remove_block 0

clean-library:
		rm -f *.so
		rm -f *.a
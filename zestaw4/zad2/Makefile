CC = gcc -Wall -std=c11
all: test

test: main.c test_sig.c child.c defines.h
		$(CC) -o test main.c
		$(CC) -o test_sig test_sig.c
		$(CC) -o child child.c
		./test
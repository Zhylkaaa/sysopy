CC = gcc -Wall

all: compile test

compile: main.c
		$(CC) -o test main.c

test: main.c test.txt
		$(CC) -o test main.c
		./test test.txt
CC = gcc -Wall -std=c11 -ggdb

all: test

test: main.c child.c
		$(CC) -o main main.c
		$(CC) -o child child.c
		./main lists 10 10 1 1 1
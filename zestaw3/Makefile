CC = gcc -Wall -std=c11 -ggdb

all: test

test: main.c
		$(CC) -o main main.c
		$(CC) -o child child.c
		./main lists 2 1000 0
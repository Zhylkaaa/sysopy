CC = gcc -Wall -std=c11 -ggdb

all: test clean

test: main.c
		$(CC) -o main main.c
		./main test
		./main test -maxdepth 2

clean:
		rm -f main
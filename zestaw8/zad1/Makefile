CC = gcc -Wall

all: compile test

compile: histogram.c
		$(CC) -o histogram histogram.c
test: histogram.c
	./histogram 2 block brain_604.ascii.pgm cokolwiek.txt
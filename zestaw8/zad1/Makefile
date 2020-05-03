CC = gcc -Wall

all: compile test

compile: histogram.c
		$(CC) -o histogram histogram.c
test: histogram.c
	./histogram 4 block dragon.ascii.pgm cokolwiek.txt
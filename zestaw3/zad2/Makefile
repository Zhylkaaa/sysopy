CC = gcc -Wall -std=c11 -ggdb

all: test

test: main.c child.c
		$(CC) -o main main.c
		$(CC) -o child child.c
		./main lists 3 10 1

generate_test: generator.c
		$(CC) -o generator generator.c
		./generator 1 1000 10000

unit_test: main.c child.c unit_test.c
		$(CC) -o main main.c
		$(CC) -o child child.c
		$(CC) -o unit_test unit_test.c
		./unit_test
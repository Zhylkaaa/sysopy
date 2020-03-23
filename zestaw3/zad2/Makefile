CC = gcc -Wall -std=c11 -ggdb

all: test

test:
		$(CC) -o main main.c
		$(CC) -o child child.c
		./main lists 10 10 0

generate_test: generator.c
		$(CC) -o generator generator.c
		./generator 1 3 5

unit_test:
		$(CC) -o main main.c
		$(CC) -o child child.c
		$(CC) -o unit_test unit_test.c
		./unit_test
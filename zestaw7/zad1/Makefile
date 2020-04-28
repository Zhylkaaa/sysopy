CC = gcc -Wall

all: compile test

compile: workerA.c workerB.c workerC.c main.c common.h
		$(CC) -o workerA workerA.c
		$(CC) -o workerB workerB.c
		$(CC) -o workerC workerC.c
		$(CC) -o run main.c

test:
		./run 1 1 1
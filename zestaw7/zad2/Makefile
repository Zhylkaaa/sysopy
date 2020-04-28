CC = gcc -Wall

all: compile test

compile: workerA.c workerB.c workerC.c main.c common.h
		$(CC) -o workerA workerA.c -lpthread
		$(CC) -o workerB workerB.c -lpthread
		$(CC) -o workerC workerC.c -lpthread
		$(CC) -o run main.c -lpthread

test:
		./run 1 1 1
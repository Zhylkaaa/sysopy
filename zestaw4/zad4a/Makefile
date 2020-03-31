CC = gcc -Wall

all: test

test: main.c sender.c catcher.c
		$(CC) -o main main.c
		$(CC) -o sender sender.c
		$(CC) -o catcher catcher.c
		./main 100 sigrt

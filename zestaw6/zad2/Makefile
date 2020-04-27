CC = gcc -Wall

all: client server

client: client.c common.h
		$(CC) -o client client.c -lrt

server: server.c common.h
		$(CC) -o server server.c -lrt
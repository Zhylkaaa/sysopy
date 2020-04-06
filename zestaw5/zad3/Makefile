CC = gcc -Wall

all: test

test: main.c producer.c consumer.c
		$(CC) -o test main.c
		$(CC) -o producer producer.c
		$(CC) -o consumer consumer.c
		./test fifo a.txt b.txt c.txt d.txt 1.txt 1 out.txt
		rm -f fifo
clear:
	rm -f fifo
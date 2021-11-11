CC = gcc
CFLAGS = -g -std=c99 -pedantic -Wall -Wextra

all: main

main: scanner.o parser.o
		$(CC) $(CFLAGS) scanner.o parser.o -o main

scanner.o: scanner.c scanner.h
		$(CC) $(CFLAGS) -c scanner.c -o scanner.o
parser.o: parser.c
		$(CC) $(CFLAGS) -c parser.c -o parser.o

clean:
	rm *.o


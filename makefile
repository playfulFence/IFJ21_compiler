CC = gcc
CFLAGS = -g -std=c99 -pedantic -Wall -Wextra

all: compiler

compiler: scanner.o parser.o compiler.o
		$(CC) $(CFLAGS) scanner.o parser.o compiler.o -o compiler
compiler.o: compiler.c
		$(CC) $(CFLAGS) -c compiler.c -o compiler.o
scanner.o: scanner.c scanner.h
		$(CC) $(CFLAGS) -c scanner.c -o scanner.o
parser.o: parser.c parser.h
		$(CC) $(CFLAGS) -c parser.c -o parser.o
clean:
	rm *.o
run: all
	./compiler


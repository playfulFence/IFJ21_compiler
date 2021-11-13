CC = gcc
CFLAGS = -g -std=c99 -pedantic -Wall -Wextra -O2

all: compiler

compiler: scanner.o parser.o compiler.o dynamic_string.o htab.o
		$(CC) $(CFLAGS) scanner.o parser.o compiler.o dynamic_string.o htab.o -o compiler
compiler.o: compiler.c
		$(CC) $(CFLAGS) -c compiler.c -o compiler.o
scanner.o: scanner.c scanner.h
		$(CC) $(CFLAGS) -c scanner.c -o scanner.o
parser.o: parser.c parser.h
		$(CC) $(CFLAGS) -c parser.c -o parser.o
dynamic_string.o: dynamic_string.c dynamic_string.h
		$(CC) $(CFLAGS) -c dynamic_string.c -o dynamic_string.o
htab.o: htab.c htab.h
		$(CC) $(CFLAGS) -c htab.c -o htab.o
clean:
	rm *.o
run: all
	./compiler


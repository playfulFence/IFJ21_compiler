CC = gcc
CFLAGS = -g -std=c99 -pedantic -Wall -Wextra -O2

all: compiler

compiler: scanner.o parser.o compiler.o dynamic_string.o htab.o stack_p_t_elements.o stack_p_t.o bottom_up_analysis.o tree.o
		$(CC) $(CFLAGS) scanner.o parser.o compiler.o dynamic_string.o htab.o stack_p_t_elements.o stack_p_t.o bottom_up_analysis.o tree.o -o compiler
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
bottom_up_analysis.o: bottom_up_analysis.c bottom_up_analysis.h stack_p_t_elements.o stack_p_t.o tree.o
		$(CC) $(CFLAGS) stack_p_t_elements.o stack_p_t.o tree.o -c bottom_up_analysis.c -o bottom_up_analysis.o
stack_p_t_elements.o: stack_p_t_elements.c stack_p_t_elements.h 
		$(CC) $(CFLAGS) -c stack_p_t_elements.c -o stack_p_t_elements.o
stack_p_t.o: stack_p_t.c stack_p_t.h 
		$(CC) $(CFLAGS) -c stack_p_t.c -o stack_p_t.o
tree.o: tree.c tree.h
		$(CC) $(CFLAGS) -c tree.c -o tree.o
clean:
	rm *.o
run: all
	./compiler


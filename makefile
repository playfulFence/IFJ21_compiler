#Authors : Mikhailov Kirill(xmikha00) Naumenko Maksim(xnaume01)

#Projekt: Implementace překladače imperativního jazyka IFJ21


CC = gcc
CFLAGS = -g -std=c99 -pedantic -Wall -Wextra -O2

all: compiler

compiler: error.o scanner.o parser.o compiler.o dynamic_string.o symtable.o stack_p_t_elements.o stack_p_t.o tree.o bottom_up_analysis.o
		$(CC) $(CFLAGS) error.o scanner.o parser.o compiler.o dynamic_string.o symtable.o stack_p_t_elements.o stack_p_t.o tree.o bottom_up_analysis.o -o compiler
compiler.o: compiler.c
		$(CC) $(CFLAGS) -c compiler.c -o compiler.o
parser.o: parser.c parser.h scanner.h error.h
		$(CC) $(CFLAGS) -c parser.c -o parser.o
scanner.o: scanner.c scanner.h token.h error.h
		$(CC) $(CFLAGS) -c scanner.c -o scanner.o
dynamic_string.o: dynamic_string.c dynamic_string.h
		$(CC) $(CFLAGS) -c dynamic_string.c -o dynamic_string.o
symtable.o: symtable.c symtable.h
		$(CC) $(CFLAGS) -c symtable.c -o symtable.o
bottom_up_analysis.o: bottom_up_analysis.c bottom_up_analysis.h stack_p_t_elements.h stack_p_t.h tree.h token.h dynamic_string.h
		$(CC) $(CFLAGS) -c bottom_up_analysis.c -o bottom_up_analysis.o
stack_p_t_elements.o: stack_p_t_elements.c stack_p_t_elements.h 
		$(CC) $(CFLAGS) -c stack_p_t_elements.c -o stack_p_t_elements.o
stack_p_t.o: stack_p_t.c stack_p_t.h token.h
		$(CC) $(CFLAGS) -c stack_p_t.c -o stack_p_t.o
error.o: error.c error.h
		$(CC) $(CFLAGS) -c error.c -o error.o
tree.o: tree.c tree.h
		$(CC) $(CFLAGS) -c tree.c -o tree.o

clean:
	rm *.o compiler
run: all
	./compiler


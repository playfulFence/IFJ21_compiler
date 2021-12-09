// Authors : Maksim Naumenko(xnaume01)
        //   Kirill Mikhailov(xmikha00)


//Projekt: Implementace překladače imperativního jazyka IFJ21


#ifndef __PARSER_H__
#define  __PARSER_H__

#include <stdio.h>
#include "tree.h"
#include "symtable.h"
#include "token.h"
#include "bottom_up_analysis.h"


FILE* openFile(int argc, char** argv);

ast_node *parseAST(htab_t *symTable, FILE *f);


#endif // __PARSER_H__

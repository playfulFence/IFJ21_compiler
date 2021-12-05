#ifndef __PARSER_H__
#define  __PARSER_H__

#include <stdio.h>
#include "tree.h"
#include "htab.h"
#include "token.h"
#include "bottom_up_analysis.h"


FILE* openFile(int argc, char** argv);

ast_node *parseAST(htab_t *symTable, FILE *f);


#endif // __PARSER_H__

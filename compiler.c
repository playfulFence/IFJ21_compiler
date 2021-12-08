#include "parser.h"
#include "scanner.h"
#include "htab.h"
#include "dynamic_string.h"
#include "tree.h"
#include "stack_p_t.h"
#include <stdio.h>
#include <stdlib.h>
#include "string.h"

char *tokens[] = {"ID", ":", ",", "INT", "NUM", "STR", "WHILE", "DO", "IF", "THEN", "ELSE", "END","NUM_KW","INT_KW","STR_KW", "FUNC", "GLOBAL", "LOCAL", "NIL", "REQUAIRE", "RETURN", "#", "+","-","*","/","//","..","<",">","<=",">=","==","=","~=","(",")","EOF"};

void testScanner(FILE *f)
{
    DynamicString dynamicString;
    DynamicStringInit(&dynamicString);
    StackTokens tokenStack;
    initStackTokens(&tokenStack);
    DynamicStringDispose(&dynamicString);
    token_t *token = getToken(f, &dynamicString, &tokenStack);
    while (token->type != TOKEN_EOF)
    {
        if((token->type == TOKEN_ID) || (token->type == TOKEN_STR))
        {
            printf("[[TYPE: %10s || STRDATA: %10s  STRLEN: %10d]]\n", tokens[token->type], token->data.tokenStringVal);    
        }
        else if(token->type == TOKEN_INT)
        {
            printf("[[TYPE: %10s || INTDATA: %10d ]]\n", tokens[token->type], token->data.tokenIntVal);
        }
        else if(token->type == TOKEN_NUM)
        {
            printf("[[TYPE: %10s || NUMDATA: %10f ]]\n", tokens[token->type], token->data.tokenNumVal);
        }
        else
        {
            printf("[[TYPE: %10s || NONE                ]]\n", tokens[token->type]);
        }
        DynamicStringDispose(&dynamicString);   
        token = getToken(f, &dynamicString, &tokenStack);
    }
    ungetToken(token, &tokenStack);
    token = getToken(f, &dynamicString, &tokenStack);
    printf("[[TYPE: %10s || NONE                ]]\n", tokens[token->type]);
    DynamicStringDispose(&dynamicString);
    destroyStackTokens(&tokenStack);
}

int main(int argc, char** argv)
{
    // make hash table
    htab_t * symtable = htab_init(43969);
    // open file to read program
    FILE* prog = openFile(argc, argv);
   // testScanner(prog);
    //  // call parser to make abastract syntax tree
     ast_node *ast = parseAST(symtable, prog);
     printf("GDFSDFSFDSfg\n");
     printAST(ast);
    
    
    // ast_node *ast = make_new_node();
    // ast_node *child1 = make_new_node();
    // make_new_child(ast, child1);
    // ast_node *child2 = make_new_node();
    // make_new_child(ast, child2);
    // ast_node *child3 = make_new_node();
    // make_new_child(child1, child3);
    // // ast_node *child4 = make_new_node();
    // // make_new_child(ast, child4);
    // // ast_node *child5 = make_new_node();
    // // make_new_child(child3, child5);
    // //freeChildrenNodes(child3);
    // printf("Проiбався\n");
    // printAST(ast);
    
    fclose(prog);
    return 0;
}

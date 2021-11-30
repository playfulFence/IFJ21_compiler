#include "parser.h"
#include "scanner.h"
#include "htab.h"
#include "dynamic_string.h"
#include "bottom_up_analysis.h"
#include "tree.h"

int main(int argc, char** argv)
{
    // make hash table
    htab_t * symtable = htab_init(43969);
    // open file to read program
    FILE* prog = openFile(argc, argv);
    // call parser to make abastract syntax tree
    ast_node *ast = parseAST(symtable, prog);


    
    // ast_node *ast = make_new_node();
    // ast_node *child1 = make_new_node();
    // make_new_child(ast, child1);
    // ast_node *child2 = make_new_node();
    // make_new_child(ast, child2);
    // ast_node *child3 = make_new_node();
    // make_new_child(child1, child3);
    // ast_node *child4 = make_new_node();
    // make_new_child(ast, child4);
    // ast_node *child5 = make_new_node();
    // make_new_child(child3, child5);
    // printAST(ast);
    
    fclose(prog);
    return 0;
}

//char *tokens[] = {"ID", ":", ",", "INT", "NUM", "STR", "WHILE", "DO", "IF", "THEN", "ELSE", "END","NUM_KW","INT_KW","STR_KW", "FUNC", "GLOBAL", "LOCAL", "NIL", "REQUAIRE", "RETURN", "#", "+","-","*","/","//","..","<",">","<=",">=","==","=","~=","(",")","EOF"};
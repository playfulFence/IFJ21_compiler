#include "tree.h"

ast_node *make_new_node()
{
    ast_node *newNode = malloc(sizeof(ast_node));
    newNode->childrenCounter = 0;
    return newNode;
}

void make_new_child(ast_node *parentNode, ast_node *newChild)
{
    parentNode->childrenCounter++;
    parentNode->childrenNodes = realloc(parentNode->childrenNodes, parentNode->childrenCounter*sizeof(ast_node));
    parentNode->childrenNodes[parentNode->childrenCounter-1]= newChild;
}

void print_nodes(ast_node *ast, int indent)
{
    indent++;
    for (int i = 0; i < indent*2; i++)
    {   
        printf(" ");
    }
    printf("nodeType\n");
    for (int i = 0; i < ast->childrenCounter; i++)
    {
        print_nodes(ast->childrenNodes[i], indent);
    }
    
}

void printAST(ast_node *ast)
{
    printf("-------AST-------\n");
    print_nodes(ast, -1);
    printf("-----------------\n");
}


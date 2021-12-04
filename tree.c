#include "tree.h"

char *nodes[] = {"[*]", "[/]", "[//]", "[+]", "[-]", "[..]", "[<]", "[<=]", "[>]", "[>=]", "==", "~=","ID","#"};

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

void freeChildrenNodes(ast_node *parentNode)
{
    for (int i = 0; i < parentNode->childrenCounter; i++)
    {
        free(parentNode->childrenNodes[i]);
    }
    parentNode->childrenCounter = 0;
    free(parentNode->childrenNodes);
}

void print_nodes(ast_node *ast, int indent)
{
    indent++;
    for (int i = 0; i < indent*2; i++)
    {   
        printf(" ");
    }
    if(ast->nodeType == NODE_ID)
    {
        if(ast->nodeData.intData != 0)
        {
            printf("%s(%d)\n", nodes[ast->nodeType], ast->nodeData.intData);
        }
        else if(ast->nodeData.doubleData != 0)
        {
            printf("%s(%f)\n", nodes[ast->nodeType], ast->nodeData.doubleData);
        }
        else if(ast->nodeData.stringData != NULL)
        {
            printf("%s(%s)\n", nodes[ast->nodeType], ast->nodeData.stringData);
        }
    }
    else
    {
        printf("%s\n", nodes[ast->nodeType]);
    }
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


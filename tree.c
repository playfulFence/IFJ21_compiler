#include "tree.h"

char *nodes[] = {"[*]", "[/]", "[//]", "[+]", "[-]", "[..]", "[<]", "[<=]", "[>]", "[>=]", "==", "~=","ID","#","FUNC_DEF","FUNC_DEF_PARAM_LIST", "FUNC_DEF_RETURN_DATATYPES_LIST", "FUNC_VOID_CALL", "FUNC_CALL", "FUNC_CALL_PARAM_LIST", "FUNC_DECL", "FUNC_DECL_DATATYPES_LIST", "FUNC_DECL_RETURN_DATATYPES_LIST", "STATEMENTS_LIST", "STATEMENT", "IF", "IF_CONDITION", "IF_THEN", "IF_ELSE", "THEN", "ELSE", "WHILE", "WHILE_CONDITION", "WHILE_DO", "RETURN", "VAR_DEF", "SINGLE_ASSIGN", "IDS", "VALUES", "INT_ARG", "NUM_ARG", "STR_ARG", "NIL_ARG", "ZERO_ARG", "ID_ARG", "MULTIPLE_ASSIGN", "PROG"};

ast_node *make_new_node()
{
    ast_node *newNode = malloc(sizeof(ast_node));
    newNode->childrenNodes = malloc(sizeof(ast_node));
    newNode->childrenCounter = 0;
    //newNode->nodeData.nilFlag = false;
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
    if(ast->nodeType == NODE_ID || ast->nodeType == NODE_NIL_ARG)
    {
        if(ast->hashTableItem->varIntVal != 0)
        {
            printf("%s(%d)\n", nodes[ast->nodeType], ast->hashTableItem->varIntVal);
        }
        else if(ast->hashTableItem->varIntVal != 0)
        {
            printf("%s(%f)\n", nodes[ast->nodeType], ast->hashTableItem->varNumVal);
        }
        else if(ast->nodeData.stringData != NULL)
        {
            char *ch = ast->nodeData.stringData;
            printf("%s(%s)\n", nodes[ast->nodeType], ch);
        }
    }
    else if(ast->nodeType == NODE_INT_ARG || ast->nodeType == NODE_NUM_ARG)
    {
        printf("%s(%.2f)\n", nodes[ast->nodeType], ast->hashTableItem->varNumVal);
    }
    else if(ast->nodeType == NODE_STR_ARG)
    {
        printf("%s(%s)\n", nodes[ast->nodeType], ast->hashTableItem->varStrVal);
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


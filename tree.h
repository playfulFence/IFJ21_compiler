#ifndef __TREE_H__
#define  __TREE_H__

#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"

typedef enum treeNodeTypes
{
    NODE_MULT,     // *
    NODE_DIV,      // /
    NODE_INTDIV,   // //
    NODE_PLUS,     // +
    NODE_MINUS,    // -
    NODE_CONC,     // ..
    NODE_LESS,     // <
    NODE_LEQ,      // <=
    NODE_GREATER,  // >
    NODE_GEQ,      // >=
    NODE_EQUAL,    // ==
    NODE_NEQ,      // ~=
    NODE_ID,       // ID
    NODE_FUNC_DEF,  //
    NODE_FUNC_DEF_PARAM_LIST,
    NODE_FUNC_DEF_RETURN_DATATYPES_LIST,
    NODE_FUNC_CALL, 
    NODE_FUNC_CALL_PARAM_LIST,
    NODE_FUNC_DECL,
    NODE_FUNC_DECL_DATATYPES_LIST,
    NODE_FUNC_DECL_RETURN_DATATYPES_LIST,
    NODE_STATEMENTS_LIST,
    NODE_STATEMENT,
    NODE_IF,
    NODE_THEN,
    NODE_ELSE,
    NODE_WHILE,
    NODE_WHILE_DO,
    NODE_RETURN,
    NODE_VAR_DEF,
    NODE_ASSIGN
}treeNodeType;

typedef struct treeNodes
{
    treeNodeType type;
    token_t token;
    struct treeNodes **childrenNodes;
} treeNode;

void treeInit(treeNode**);
void treeInsert(treeNode**, treeNodeType, token_t);



#endif // __TREE_H__

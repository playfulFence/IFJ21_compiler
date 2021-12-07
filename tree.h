#ifndef __TREE_H__
#define  __TREE_H__

#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "htab.h"

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
    NODE_STRLEN,   // #
    NODE_FUNC_DEF,  //
    NODE_FUNC_DEF_PARAM_LIST,
    NODE_FUNC_DEF_RETURN_DATATYPES_LIST,
    NODE_FUNC_VOID_CALL,
    NODE_FUNC_CALL, 
    NODE_FUNC_CALL_PARAM_LIST,
    NODE_FUNC_DECL,
    NODE_FUNC_DECL_DATATYPES_LIST,
    NODE_FUNC_DECL_RETURN_DATATYPES_LIST,
    NODE_STATEMENTS_LIST,
    NODE_STATEMENT,
    NODE_IF,
    NODE_IF_CONDITION,
    NODE_IF_THEN,
    NODE_IF_ELSE,
    NODE_THEN,
    NODE_ELSE,
    NODE_WHILE,
    NODE_WHILE_CONDITION,
    NODE_WHILE_DO,
    NODE_RETURN,
    NODE_VAR_DEF,
    NODE_SINGLE_ASSIGN,
    NODE_IDS,
    NODE_VALUES,
    NODE_INT_ARG,
    NODE_NUM_ARG,
    NODE_STR_ARG,
    NODE_NIL_ARG,
    NODE_ZERO_ARG,
    NODE_ID_ARG,
    NODE_MULTIPLE_ASSIGN,
    NODE_PROG,
    NODE_NIL,
    NODE_TRUE
}treeNodeType;
typedef struct abstract_syntax_tree_node
{
    treeNodeType nodeType; // Type of the node
    union Data             // Node's data
    {
        int intData;
        double doubleData;
        char *stringData;
        bool zeroFlag;
        bool nilFlag;
    }nodeData;
    htab_data_t *hashTableItem; // Pointer to hashTable element
    int childrenCounter; // Amount of children 
    struct abstract_syntax_tree_node **childrenNodes; // List of children
}ast_node;

// creates new empty node 
ast_node *make_new_node();

void make_new_child(ast_node *parentNode, ast_node *newChild);

void printAST(ast_node *ast);

void print_nodes(ast_node *ast, int indent);

void freeChildrenNodes(ast_node *);




#endif // __TREE_H__

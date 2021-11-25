#ifndef __STACK_P_T_ELEMENTS__H__
#define __STACK_P_T_ELEMENTS__H__

#include <stdio.h>
#include <stdlib.h>
//#include "bottom_up_analysis.h"

// elements that are used for bottom-up analysis
typedef enum ptElements
{
    STRLEN,
    MULT,
    DIV,
    INTDIV,
    PLUS,
    MINUS,
    CONC,
    LESS,
    LEQ,
    GREATER,
    GEQ,
    EQ,
    NEQ,
    LB,
    RB,
    I,
    DOLLAR,
    REDUCE_FLAG,
    E
} ptElement;

// Element structure
typedef struct ptElementtt
{
    ptElement element;
    struct ptElementtt *nextElement;
} *ptElementPtr;

// Stack structure
typedef struct precedenceTableStack
{
    ptElementPtr top; 
} PTStack;

// Function to initialize the stack
void initStackPTElements(PTStack*);
// Function to push a new element to the stack 
void pushPTElement(PTStack*, ptElement);
// Function to pop an element from the stack
ptElement popPTElement(PTStack*);
// Function to destroy the stack 
void destroyPTStack(PTStack*);

#endif // __STACK_P_T_ELEMENTS__H__

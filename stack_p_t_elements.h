#ifndef __STACK_P_T_ELEMENTS__H__
#define __STACK_P_T_ELEMENTS__H__

#include <stdio.h>
#include <stdlib.h>

// elements that are used for bottom-up analysis
typedef enum noneTerminals
{
    NONE_TERMINAL_STRLEN,
    NONE_TERMINAL_MULT,
    NONE_TERMINAL_DIV,
    NONE_TERMINAL_INTDIV,
    NONE_TERMINAL_PLUS,
    NONE_TERMINAL_MINUS,
    NONE_TERMINAL_CONC,
    NONE_TERMINAL_LESS,
    NONE_TERMINAL_LEQ,
    NONE_TERMINAL_GREATER,
    NONE_TERMINAL_GEQ,
    NONE_TERMINAL_EQ,
    NONE_TERMINAL_NEQ,
    NONE_TERMINAL_LB,
    NONE_TERMINAL_RB,
    NONE_TERMINAL_ID,
    NONE_TERMINAL_DOLLAR,
    NONE_TERMINAL_REDUCE_FLAG,
    NONE_TERMINAL_E
} NoneTerminal;

// Element structure
typedef struct noneTerminalElement
{
    NoneTerminal element;
    struct noneTerminalElement *nextElement;
} *NoneTerminalElementPtr;

// Stack structure
typedef struct precedenceTableStack
{
    NoneTerminalElementPtr top; 
} NoneTerminalStack;

// Function to initialize the stack
void initNoneTerminalStack(NoneTerminalStack*);
// Function to push a new element to the stack 
void pushNoneTerminalElement(NoneTerminalStack*, NoneTerminal);
// Function to pop an element from the stack
NoneTerminal popNoneTerminalElement(NoneTerminalStack*);
// Function to destroy the stack 
void destroyNoneTerminalStack(NoneTerminalStack*);

#endif // __STACK_P_T_ELEMENTS__H__

// Authors : Maksim Naumenko(xnaume01)

//Projekt: Implementace překladače imperativního jazyka IFJ21


#ifndef __STACK_P_T__
#define __STACK_P_T__

#include <stdio.h>
#include <stdlib.h>
#include "token.h"

// Element structure
typedef struct Element
{
    token_t *token;
    struct Element *nextElement;
} *ElementPtr;

// Stack structure
typedef struct
{
    ElementPtr top; 
}StackTokens;

// Function to initialize the stack
void initStackTokens(StackTokens*);
// Function to push a new element to the stack 
void pushTokenStackTokens(StackTokens*, token_t*);
// Function to pop an element from the stack
token_t *popTokenStackTokens(StackTokens*);
// Function to destroy the stack 
void destroyStackTokens(StackTokens*);
// Function to detect if stack is empty
int isEmptyStackTokens(StackTokens*);



#endif // __STACK_P_T__

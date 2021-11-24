#ifndef __STACK_P_T__
#define __STACK_P_T__

#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"

// Element structure
typedef struct Element
{
    token_t token;
    struct Element *nextElement;
} *ElementPtr;

// Stack structure
typedef struct
{
    ElementPtr top; 
}Stack;

// Function to initialize the stack
void initStack(Stack*);
// Function to push a new element to the stack 
void push(Stack*, token_t);
// Function to pop an element from the stack
void pop(Stack*);
// Function to destroy the stack 
void destroy(Stack*);



#endif // __STACK_P_T__

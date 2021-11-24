#ifndef __STACK_P_T_ELEMENTS__
#define __STACK_P_T_ELEMENTS__

#include <stdio.h>
#include <stdlib.h>
#include "bottom_up_analysis.h"

// Element structure
typedef struct Element
{
    ptElement element;
    struct Element *nextElement;
} *ElementPtr;

// Stack structure
typedef struct
{
    ElementPtr top; 
}PTStack;

// Function to initialize the stack
void initStackPTElements(PTStack*);
// Function to push a new element to the stack 
void pushPTElement(PTStack*, ptElement);
// Function to pop an element from the stack
void popPTElement(PTStack*);
// Function to destroy the stack 
void destroyPTStack(PTStack*);



#endif // __STACK_P_T_ELEMENTS__
#ifndef __STACK_P_T_ELEMENTS__H__
#define __STACK_P_T_ELEMENTS__H__

#include <stdio.h>
#include <stdlib.h>
#include "bottom_up_analysis.h"

// Element structure
typedef struct ptElement
{
    ptElement element;
    struct Element *nextElement;
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

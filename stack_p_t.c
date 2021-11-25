#include "stack_p_t.h"

// Function to initialize the stack
void initStackTokens(StackTokens* stack)
{
    stack->top = NULL;
}
// Function to push a new element to the stack 
void pushToken(StackTokens* stack, token_t token)
{
    ElementPtr newElement = malloc(sizeof(ElementPtr));
    newElement->token = token;
    newElement->nextElement = NULL;
    if(stack->top == NULL)
    {
        stack->top = newElement;
    }
    else
    {
        newElement->nextElement = stack->top;
        stack->top = newElement;
    }
}
// Function to pop an element from the stack
void popToken(StackTokens* stack)
{
    if(stack->top->nextElement == NULL)
    {
        free(stack->top);
        stack->top = NULL;
    }
    else
    {
        ElementPtr tmp = stack->top;
        stack->top = tmp->nextElement;
        free(tmp);
    }
}
// Function to destroy the stack 
void destroyStackTokens(StackTokens* stack)
{
    while (stack->top != NULL)
    {
        popToken(stack);
    } 
}

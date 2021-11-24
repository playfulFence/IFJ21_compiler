#include "stack_p_t.h"

// Function to initialize the stack
void initStack(Stack* stack)
{
    stack->top = NULL;
}
// Function to push a new element to the stack 
void push(Stack* stack, token_t token)
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
void pop(Stack* stack)
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
void destroy(Stack* stack)
{
    while (stack->top != NULL)
    {
        pop(stack);
    }
    
}
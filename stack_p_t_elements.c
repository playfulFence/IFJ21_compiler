#include "stack_p_t_elements.h"

// Function to initialize the stack
void initStackPTElements(PTStack* stack)
{
    stack->top = NULL;
}
// Function to push a new element to the stack 
void pushPTElement(PTStack* stack, ptElement element)
{
    ptElementPtr newElement = malloc(sizeof(ptElementPtr));
    newElement->element = element;
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
ptElement popPTElement(PTStack* stack)
{
    ptElement element;
    if(stack->top->nextElement == NULL)
    {
        element = stack->top->element;
        free(stack->top);
        stack->top = NULL;
    }
    else
    {
        ptElementPtr tmp = stack->top;
        element = tmp->element;
        stack->top = tmp->nextElement;
        free(tmp);
    }
    return element;
}
// Function to destroy the stack 
void destroyPTStack(PTStack* stack)
{
    while (stack->top != NULL)
    {
        popPTElement(stack);
    } 
}
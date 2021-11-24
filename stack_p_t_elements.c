#include "stack_p_t_elements.h"

// Function to initialize the stack
void initStackPTElements(PTStack* stack)
{
    stack->top = NULL;
}
// Function to push a new element to the stack 
void pushPTElement(PTStack* stack, ptElement element)
{
    ElementPtr newElement = malloc(sizeof(ElementPtr));
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
void popPTElement(PTStack* stack)
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
void destroyPTStack(PTStack* stack)
{
    while (stack->top != NULL)
    {
        pop(stack);
    } 
}
// Authors : Maksim Naumenko(xnaume01)

//Projekt: Implementace překladače imperativního jazyka IFJ21


#include "stack_p_t_elements.h"

// Function to initialize the stack
void initNoneTerminalStack(NoneTerminalStack* stack)
{
    stack->top = NULL;
}
// Function to push a new element to the stack 
void pushNoneTerminalElement(NoneTerminalStack* stack, NoneTerminal element)
{
    NoneTerminalElementPtr newElement = malloc(sizeof(NoneTerminalElementPtr));
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
NoneTerminal popNoneTerminalElement(NoneTerminalStack* stack)
{
    NoneTerminal element;
    if(stack->top->nextElement == NULL)
    {
        element = stack->top->element;
        free(stack->top);
        stack->top = NULL;
    }
    else
    {
        NoneTerminalElementPtr tmp = stack->top;
        element = tmp->element;
        stack->top = tmp->nextElement;
        free(tmp);
    }
    return element;
}
// Function to destroy the stack 
void destroyNoneTerminalStack(NoneTerminalStack* stack)
{
    while (stack->top != NULL)
    {
        popNoneTerminalElement(stack);
    } 
}

#include "scanner.h"
#include "stack_p_t.h"
#include "stack_p_t_elements.h"
#include "dynamic_string.h"
#include "tree.h"

ptElement ptFromTokenToPTElement(tokenType_t type)
{
    ptElement element;
    switch (type)
    {
        case TOKEN_STRLEN:
            element = STRLEN;
            break;
        case TOKEN_MULT:
            element = MULT;
            break;
        case TOKEN_S_BS:
            element = DIV;
            break;
        case TOKEN_D_BS:
            element = INTDIV;
            break;
        case TOKEN_PLUS:
            element = PLUS;
            break;
        case TOKEN_MINUS:
            element = MINUS;
            break;
        case TOKEN_CONC:
            element = CONC;
            break;
        case TOKEN_LESS:
            element = LESS;
            break;
        case TOKEN_LESS_EQ:
            element = LEQ;
            break;
        case TOKEN_GREAT:
            element = GREATER;
            break;
        case TOKEN_GREAT_EQ:
            element = GEQ;
            break;
        case TOKEN_EQ:
            element = EQ;
            break;
        case TOKEN_NEQ:
            element = NEQ;
            break;
        case TOKEN_L_BR:
            element = LB;
            break;
        case TOKEN_R_BR:
            element = RB;
            break;
        case TOKEN_ID:
        case TOKEN_INT:
        case TOKEN_NUM:
        case TOKEN_STR:
            element = I;
            break;
        default:
            element = DOLLAR;
            break;
    }
    return element;
}

void reduceByTheRule(PTStack *ptElementsStack, DynamicString *ruleSequenceString, ptElement first)
{
    ptElement noneTerminal;
    switch (first)
    {
    case I: // E --> id
        noneTerminal = popPTElement(ptElementsStack);
        if(noneTerminal == I)
        {
            noneTerminal = popPTElement(ptElementsStack);
            if(noneTerminal == REDUCE_FLAG)
            {
                pushPTElement(ptElementsStack, E);
                DynamicStringInsertLast(ruleSequenceString, '0');
            }
        }
        break;
    case STRLEN: // E --> #E
        noneTerminal = popPTElement(ptElementsStack);
        if(noneTerminal == E)
        {
            noneTerminal = popPTElement(ptElementsStack);
            if(noneTerminal == STRLEN)
            {
                noneTerminal = popPTElement(ptElementsStack);
                if(noneTerminal == REDUCE_FLAG)
                {
                    pushPTElement(ptElementsStack, E);
                    DynamicStringInsertLast(ruleSequenceString, '1');
                }
            }
        }
        break;
    case MULT: // E --> E*E
        noneTerminal = popPTElement(ptElementsStack);
        if(noneTerminal == E)
        {
            noneTerminal = popPTElement(ptElementsStack);
            if(noneTerminal == MULT)
            {
                noneTerminal = popPTElement(ptElementsStack);
                if(noneTerminal == E)
                {
                    noneTerminal = popPTElement(ptElementsStack);
                    if(noneTerminal == REDUCE_FLAG)
                    {
                        pushPTElement(ptElementsStack, E);
                        DynamicStringInsertLast(ruleSequenceString, '2');
                    }                    
                }
            }
        }
        break;
    case DIV: // E --> E/E
        noneTerminal = popPTElement(ptElementsStack);
        if(noneTerminal == E)
        {
            noneTerminal = popPTElement(ptElementsStack);
            if(noneTerminal == DIV)
            {
                noneTerminal = popPTElement(ptElementsStack);
                if(noneTerminal == E)
                {
                    noneTerminal = popPTElement(ptElementsStack);
                    if(noneTerminal == REDUCE_FLAG)
                    {
                        pushPTElement(ptElementsStack, E);
                        DynamicStringInsertLast(ruleSequenceString, '3');
                    }                    
                }
            }
        }
        break;
    case INTDIV: // E --> E//E
        noneTerminal = popPTElement(ptElementsStack);
        if(noneTerminal == E)
        {
            noneTerminal = popPTElement(ptElementsStack);
            if(noneTerminal == INTDIV)
            {
                noneTerminal = popPTElement(ptElementsStack);
                if(noneTerminal == E)
                {
                    noneTerminal = popPTElement(ptElementsStack);
                    if(noneTerminal == REDUCE_FLAG)
                    {
                        pushPTElement(ptElementsStack, E);
                        DynamicStringInsertLast(ruleSequenceString, '4');
                    }                    
                }
            }
        }
        break;
    case PLUS: // E --> E+E
        noneTerminal = popPTElement(ptElementsStack);
        if(noneTerminal == E)
        {
            noneTerminal = popPTElement(ptElementsStack);
            if(noneTerminal == PLUS)
            {
                noneTerminal = popPTElement(ptElementsStack);
                if(noneTerminal == E)
                {
                    noneTerminal = popPTElement(ptElementsStack);
                    if(noneTerminal == REDUCE_FLAG)
                    {
                        pushPTElement(ptElementsStack, E);
                        DynamicStringInsertLast(ruleSequenceString, '5');
                    }                    
                }
            }
        }
        break;
    case MINUS: // E --> E-E
        noneTerminal = popPTElement(ptElementsStack);
        if(noneTerminal == E)
        {
            noneTerminal = popPTElement(ptElementsStack);
            if(noneTerminal == MINUS)
            {
                noneTerminal = popPTElement(ptElementsStack);
                if(noneTerminal == E)
                {
                    noneTerminal = popPTElement(ptElementsStack);
                    if(noneTerminal == REDUCE_FLAG)
                    {
                        pushPTElement(ptElementsStack, E);
                        DynamicStringInsertLast(ruleSequenceString, '6');
                    }                    
                }
            }
        }
        break;
    case CONC: // E --> E..E
        noneTerminal = popPTElement(ptElementsStack);
        if(noneTerminal == E)
        {
            noneTerminal = popPTElement(ptElementsStack);
            if(noneTerminal == CONC)
            {
                noneTerminal = popPTElement(ptElementsStack);
                if(noneTerminal == E)
                {
                    noneTerminal = popPTElement(ptElementsStack);
                    if(noneTerminal == REDUCE_FLAG)
                    {
                        pushPTElement(ptElementsStack, E);
                        DynamicStringInsertLast(ruleSequenceString, '7');
                    }                    
                }
            }
        }
        break;
    case LESS: // E --> E<E
        noneTerminal = popPTElement(ptElementsStack);
        if(noneTerminal == E)
        {
            noneTerminal = popPTElement(ptElementsStack);
            if(noneTerminal == LESS)
            {
                noneTerminal = popPTElement(ptElementsStack);
                if(noneTerminal == E)
                {
                    noneTerminal = popPTElement(ptElementsStack);
                    if(noneTerminal == REDUCE_FLAG)
                    {
                        pushPTElement(ptElementsStack, E);
                        DynamicStringInsertLast(ruleSequenceString, '8');
                    }                    
                }
            }
        }
        break;
    case LEQ: // E --> E<=E
        noneTerminal = popPTElement(ptElementsStack);
        if(noneTerminal == E)
        {
            noneTerminal = popPTElement(ptElementsStack);
            if(noneTerminal == LEQ)
            {
                noneTerminal = popPTElement(ptElementsStack);
                if(noneTerminal == E)
                {
                    noneTerminal = popPTElement(ptElementsStack);
                    if(noneTerminal == REDUCE_FLAG)
                    {
                        pushPTElement(ptElementsStack, E);
                        DynamicStringInsertLast(ruleSequenceString, '9');
                    }                    
                }
            }
        }
        break;
    case GREATER: // E --> E>E
        noneTerminal = popPTElement(ptElementsStack);
        if(noneTerminal == E)
        {
            noneTerminal = popPTElement(ptElementsStack);
            if(noneTerminal == GREATER)
            {
                noneTerminal = popPTElement(ptElementsStack);
                if(noneTerminal == E)
                {
                    noneTerminal = popPTElement(ptElementsStack);
                    if(noneTerminal == REDUCE_FLAG)
                    {
                        pushPTElement(ptElementsStack, E);
                        DynamicStringInsertLast(ruleSequenceString, 'A');
                    }                    
                }
            }
        }
        break;
    case GEQ: // E --> E>=E
        noneTerminal = popPTElement(ptElementsStack);
        if(noneTerminal == E)
        {
            noneTerminal = popPTElement(ptElementsStack);
            if(noneTerminal == GEQ)
            {
                noneTerminal = popPTElement(ptElementsStack);
                if(noneTerminal == E)
                {
                    noneTerminal = popPTElement(ptElementsStack);
                    if(noneTerminal == REDUCE_FLAG)
                    {
                        pushPTElement(ptElementsStack, E);
                        DynamicStringInsertLast(ruleSequenceString, 'B');
                    }                    
                }
            }
        }
        break;
    case EQ: // E --> E==E
        noneTerminal = popPTElement(ptElementsStack);
        if(noneTerminal == E)
        {
            noneTerminal = popPTElement(ptElementsStack);
            if(noneTerminal == EQ)
            {
                noneTerminal = popPTElement(ptElementsStack);
                if(noneTerminal == E)
                {
                    noneTerminal = popPTElement(ptElementsStack);
                    if(noneTerminal == REDUCE_FLAG)
                    {
                        pushPTElement(ptElementsStack, E);
                        DynamicStringInsertLast(ruleSequenceString, 'C');
                    }                    
                }
            }
        }
        break;
    case NEQ: // E --> E~=E
        noneTerminal = popPTElement(ptElementsStack);
        if(noneTerminal == E)
        {
            noneTerminal = popPTElement(ptElementsStack);
            if(noneTerminal == NEQ)
            {
                noneTerminal = popPTElement(ptElementsStack);
                if(noneTerminal == E)
                {
                    noneTerminal = popPTElement(ptElementsStack);
                    if(noneTerminal == REDUCE_FLAG)
                    {
                        pushPTElement(ptElementsStack, E);
                        DynamicStringInsertLast(ruleSequenceString, 'D');
                    }                    
                }
            }
        }
        break;
    case RB: // E --> (E)
        noneTerminal = popPTElement(ptElementsStack);
        if(noneTerminal == RB)
        {
            noneTerminal = popPTElement(ptElementsStack);
            if(noneTerminal == E)
            {
                noneTerminal = popPTElement(ptElementsStack);
                if(noneTerminal == LB)
                {
                    noneTerminal = popPTElement(ptElementsStack);
                    if(noneTerminal == REDUCE_FLAG)
                    {
                        pushPTElement(ptElementsStack, E);
                        DynamicStringInsertLast(ruleSequenceString, 'E');
                    }                    
                }
            }
        }
        break;
    default:
        break;
    }
}

void shiftElement(StackTokens *tokensStack, PTStack *ptElementsStack, token_t token, ptElement second)
{
    switch (second)
    {
    case I:
        // Push reduce flag and  I, and also push token to token stack
        pushPTElement(ptElementsStack, REDUCE_FLAG);
        pushPTElement(ptElementsStack, second);
        pushToken(tokensStack, token);
        break;
    case STRLEN:
    case LB:
        // Push reduce flag and LB
        pushPTElement(ptElementsStack, REDUCE_FLAG);
        pushPTElement(ptElementsStack, second);
        break;
    case RB:
        // Push RB
        pushPTElement(ptElementsStack, second);
        break;
    default:
        // Get E from stack
        ptElement noneTerminal;
        noneTerminal = popPTElement(ptElementsStack);
        // push reduce flag
        pushPTElement(ptElementsStack, REDUCE_FLAG);
        // push E
        pushPTElement(ptElementsStack, noneTerminal);
        // push operator
        pushPTElement(ptElementsStack, second);
        break;
    }
}

void bottomUpAnalysis(token_t token)
{
    // initialize stack for tokens
    StackTokens tokensStack;
    initStackTokens(&tokensStack);
    // initialize stack for precedence table elements
    PTStack ptElementsStack;
    initStackPTElements(&ptElementsStack);
    // initialize dynamic string for rule sequence 
    DynamicString ruleSequenceString;
    DynamicStringInit(&ruleSequenceString);
    // variable for the top element of precedence table elements stack
    ptElement ptStackTopElement;
    // variable for next element
    ptElement nextElement;
    // variable for previous element
    ptElement previousElement;
    // push $ to the precedence table elements stack
    pushPTElement(&ptElementsStack, DOLLAR);
    ptStackTopElement = DOLLAR;
    previousElement = DOLLAR;
    while(1)
    {
        nextElement = ptFromTokenToPTElement(token.type);
        if(nextElement == DOLLAR)
        {
            // end of expression, need to check if there is not error
            // we should have sequence of rules
            // go out from cycle and work with sequence and tree
            printf("END of EXPRESSION\n");
            break;
        }
        switch(precedenceTable[ptStackTopElement][nextElement])
        {
            case 0: // no rule
                printf("No rule\n");
                break;
            case 1: // reduce    // maybe we can detect errrors here
                reduceByTheRule(&ptElementsStack, &ruleSequenceString, ptStackTopElement);
                ptStackTopElement = previousElement;
                shiftElement(&tokensStack, &ptElementsStack, token, nextElement);
                previousElement = ptStackTopElement;
                ptStackTopElement = nextElement;
                break;
            case 2: // shift
                shiftElement(&tokensStack, &ptElementsStack, token, nextElement);
                previousElement = ptStackTopElement;
                ptStackTopElement = nextElement;
                break;
            case 3: // equal
                shiftElement(&tokensStack, &ptElementsStack, token, nextElement);
                previousElement = ptStackTopElement;
                ptStackTopElement = nextElement;
                break;
        }

    }

}

#include "bottom_up_analysis.h"
#include "scanner.h"



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
    printf("Rule first element: %d\n", first);
    ptElement noneTerminal;
    switch (first)
    {
    case I: // E --> id
        printf("1\n");
        noneTerminal = popPTElement(ptElementsStack);
        printf("Stack elemnt: %d\n", noneTerminal);
        if(noneTerminal == I)
        {
            printf("2\n");
            noneTerminal = popPTElement(ptElementsStack);
            if(noneTerminal == REDUCE_FLAG)
            {
                printf("3\n");
                pushPTElement(ptElementsStack, E);
                DynamicStringInsertLast(ruleSequenceString, '0');
            }
        }
        break;
    case STRLEN: // E --> #E
        printf("hmmmmmmmmm\n");
        noneTerminal = popPTElement(ptElementsStack);
        printf("hmmmmmmmmm\n");
        if(noneTerminal == E)
        {
            printf("hmmmmmmmmm\n");
            noneTerminal = popPTElement(ptElementsStack);
            if(noneTerminal == STRLEN)
            {
                printf("hmmmmmmmmm\n");
                noneTerminal = popPTElement(ptElementsStack);
                if(noneTerminal == REDUCE_FLAG)
                {
                    printf("hmmmmmmmmm\n");
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
    ptElement noneTerminal;
    switch (second)
    {
    case I:
        // Push reduce flag and  I, and also push token to token stack
        printf("push I\n");
        pushPTElement(ptElementsStack, REDUCE_FLAG);
        pushPTElement(ptElementsStack, second);
        pushToken(tokensStack, token);
        break;
    case STRLEN:
        printf("psuh strlen\n");
        pushPTElement(ptElementsStack, REDUCE_FLAG);
        pushPTElement(ptElementsStack, second);
        break;
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

void bottomUpAnalysis(token_t token, FILE *f)
{
    // initialize stack for tokens
    StackTokens tokensStack;
    initStackTokens(&tokensStack);
    // initialize stack for precedence table elements
    PTStack ptElementsStack;
    initStackPTElements(&ptElementsStack);

    // pls help me
    PTStack hope;
    initStackPTElements(&hope);

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

    pushPTElement(&hope, DOLLAR);

    ptStackTopElement = DOLLAR;
    previousElement = DOLLAR;
    nextElement = ptFromTokenToPTElement(token.type);
    while(1)
    {
         printf("----previousElement: %d\n nextElement: %d\n stackTopElement: %d\n\n", previousElement, nextElement, ptStackTopElement);
        if(nextElement == DOLLAR && ptStackTopElement == DOLLAR)
        {
            // end of expression, need to check if there is not error
            // we should have sequence of rules
            // go out from cycle and work with sequence and tree
            printf("END of EXPRESSION\n");
            char *strRule = DynamicStringToString(&ruleSequenceString);
            printf("ruleSequence: %s\n", strRule);
            exit(2);
        }
        if(precedenceTable[ptStackTopElement][nextElement] == 0)
        {
            printf("No rule\n");
        }
        else if(precedenceTable[ptStackTopElement][nextElement] == 1)
        {
            printf("reduce\n");
            reduceByTheRule(&ptElementsStack, &ruleSequenceString, ptStackTopElement);
            popPTElement(&hope);
            ptStackTopElement = popPTElement(&hope);
            pushPTElement(&hope, ptStackTopElement);

            continue;
        }
        else if(precedenceTable[ptStackTopElement][nextElement] == 2)
        {
            printf("shift\n");
            shiftElement(&tokensStack, &ptElementsStack, token, nextElement);
            //
            pushPTElement(&hope, nextElement);

            ptStackTopElement = nextElement;
        }
        else if(precedenceTable[ptStackTopElement][nextElement] == 3)
        {
            shiftElement(&tokensStack, &ptElementsStack, token, nextElement);
            //
            pushPTElement(&hope, nextElement);

            ptStackTopElement = nextElement;
        }
        token = getToken(f);
        nextElement = ptFromTokenToPTElement(token.type);
    }

}

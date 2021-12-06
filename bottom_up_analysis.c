#include "bottom_up_analysis.h"
#include "string.h"
#include "stdio.h"
#include "error.h"

NoneTerminal transformTokenToNoneTerminal(tokenType_t type)
{
    NoneTerminal element;
    switch (type)
    {
        case TOKEN_STRLEN:
            element = NONE_TERMINAL_STRLEN;
            break;
        case TOKEN_MULT:
            element = NONE_TERMINAL_MULT;
            break;
        case TOKEN_S_BS:
            element = NONE_TERMINAL_DIV;
            break;
        case TOKEN_D_BS:
            element = NONE_TERMINAL_INTDIV;
            break;
        case TOKEN_PLUS:
            element = NONE_TERMINAL_PLUS;
            break;
        case TOKEN_MINUS:
            element = NONE_TERMINAL_MINUS;
            break;
        case TOKEN_CONC:
            element = NONE_TERMINAL_CONC;
            break;
        case TOKEN_LESS:
            element = NONE_TERMINAL_LESS;
            break;
        case TOKEN_LESS_EQ:
            element = NONE_TERMINAL_LEQ;
            break;
        case TOKEN_GREAT:
            element = NONE_TERMINAL_GREATER;
            break;
        case TOKEN_GREAT_EQ:
            element = NONE_TERMINAL_GEQ;
            break;
        case TOKEN_EQ:
            element = NONE_TERMINAL_EQ;
            break;
        case TOKEN_NEQ:
            element = NONE_TERMINAL_NEQ;
            break;
        case TOKEN_L_BR:
            element = NONE_TERMINAL_LB;
            break;
        case TOKEN_R_BR:
            element = NONE_TERMINAL_RB;
            break;
        case TOKEN_ID:
        case TOKEN_INT:
        case TOKEN_NUM:
        case TOKEN_STR:
            element = NONE_TERMINAL_ID;
            break;
        default:
            element = NONE_TERMINAL_DOLLAR;
            break;
    }
    return element;
}

void reduceBinaryRule(NoneTerminal *topNoneTerminal, NoneTerminalStack *stackOfNoneTerminals, DynamicString *ruleSequenceString, char ruleNumber)
{
    if(popNoneTerminalElement(stackOfNoneTerminals) == NONE_TERMINAL_E)
    {
        if(popNoneTerminalElement(stackOfNoneTerminals) == *topNoneTerminal)
        {
            if(popNoneTerminalElement(stackOfNoneTerminals) == NONE_TERMINAL_E)
            {
                *topNoneTerminal = stackOfNoneTerminals->top->element;
                pushNoneTerminalElement(stackOfNoneTerminals, NONE_TERMINAL_E);
                DynamicStringInsertLast(ruleSequenceString, ruleNumber);
                return;
            }
        }
    }
}

void reduceByTheRule(NoneTerminal *topNoneTerminal, NoneTerminalStack *stackOfNoneTerminals, DynamicString *ruleSequenceString)
{
    switch (*topNoneTerminal)
    {
    case NONE_TERMINAL_ID: // E --> id
        popNoneTerminalElement(stackOfNoneTerminals);
        *topNoneTerminal = stackOfNoneTerminals->top->element;
        pushNoneTerminalElement(stackOfNoneTerminals, NONE_TERMINAL_E);
        DynamicStringInsertLast(ruleSequenceString, '0');
        break;
    case NONE_TERMINAL_STRLEN: // E --> #E
        if(popNoneTerminalElement(stackOfNoneTerminals) == NONE_TERMINAL_E)
        {
            if(popNoneTerminalElement(stackOfNoneTerminals) == NONE_TERMINAL_STRLEN)
            {
                *topNoneTerminal = stackOfNoneTerminals->top->element;
                pushNoneTerminalElement(stackOfNoneTerminals, NONE_TERMINAL_E);
                DynamicStringInsertLast(ruleSequenceString, '1');
            }
        }
        break;
    case NONE_TERMINAL_MULT: // E --> E*E
        reduceBinaryRule(topNoneTerminal, stackOfNoneTerminals, ruleSequenceString, '2');
        break;
    case NONE_TERMINAL_DIV: // E --> E/E
        reduceBinaryRule(topNoneTerminal, stackOfNoneTerminals, ruleSequenceString, '3');
        break;
    case NONE_TERMINAL_INTDIV: // E --> E//E
        reduceBinaryRule(topNoneTerminal, stackOfNoneTerminals, ruleSequenceString, '4');
        break;
    case NONE_TERMINAL_PLUS: // E --> E+E
        reduceBinaryRule(topNoneTerminal, stackOfNoneTerminals, ruleSequenceString, '5');
        break;
    case NONE_TERMINAL_MINUS: // E --> E-E
        reduceBinaryRule(topNoneTerminal, stackOfNoneTerminals, ruleSequenceString, '6');
        break;
    case NONE_TERMINAL_CONC: // E --> E..E
        reduceBinaryRule(topNoneTerminal, stackOfNoneTerminals, ruleSequenceString, '7');
        break;
    case NONE_TERMINAL_LESS: // E --> E<E
        reduceBinaryRule(topNoneTerminal, stackOfNoneTerminals, ruleSequenceString, '8');
        break;
    case NONE_TERMINAL_LEQ: // E --> E<=E
        reduceBinaryRule(topNoneTerminal, stackOfNoneTerminals, ruleSequenceString, '9');
        break;
    case NONE_TERMINAL_GREATER: // E --> E>E
        reduceBinaryRule(topNoneTerminal, stackOfNoneTerminals, ruleSequenceString, 'A');
        break;
    case NONE_TERMINAL_GEQ: // E --> E>=E
        reduceBinaryRule(topNoneTerminal, stackOfNoneTerminals, ruleSequenceString, 'B');
        break;
    case NONE_TERMINAL_EQ: // E --> E==E
        reduceBinaryRule(topNoneTerminal, stackOfNoneTerminals, ruleSequenceString, 'C');
        break;
    case NONE_TERMINAL_NEQ: // E --> E~=E
        reduceBinaryRule(topNoneTerminal, stackOfNoneTerminals, ruleSequenceString, 'D');
        break;
    case NONE_TERMINAL_RB: // E --> (E)
        if(popNoneTerminalElement(stackOfNoneTerminals) == NONE_TERMINAL_RB)
        {
            if(popNoneTerminalElement(stackOfNoneTerminals) == NONE_TERMINAL_E)
            {
                if(popNoneTerminalElement(stackOfNoneTerminals) == NONE_TERMINAL_LB)
                {
                    *topNoneTerminal = stackOfNoneTerminals->top->element;
                    pushNoneTerminalElement(stackOfNoneTerminals, NONE_TERMINAL_E);
                    DynamicStringInsertLast(ruleSequenceString, 'E');
                }
            }
        }
        break;
    default:
        break;
    }
}

void shiftElement(token_t *expressionToken, htab_list_t* hashTableList, NoneTerminal nextNoneTerminal, StackTokens *stackOfVaruables, NoneTerminalStack *stackOfNoneTerminals)
{
    pushNoneTerminalElement(stackOfNoneTerminals, nextNoneTerminal);
    if(expressionToken->type == TOKEN_STR || expressionToken->type == TOKEN_INT || expressionToken->type == TOKEN_NUM || expressionToken->type == TOKEN_ID)
    {
        if(expressionToken->type == TOKEN_ID)
        {
            if(!listSearch(hashTableList, expressionToken->data.tokenStringVal, FROM_FIRST) || !listSearch(hashTableList, expressionToken->data.tokenStringVal, FROM_FIRST)->defineFlag)
            {
                errorExit(SEMANTIC_UNDEF_REDEF_ERR, expressionToken->line);
            }

            htab_data_t* thisIDvar = listSearch(hashTableList, expressionToken->data.tokenStringVal, FROM_FIRST);


            if(thisIDvar->datatype == DATATYPE_INT) 
            {
                expressionToken->type = TOKEN_INT;
                expressionToken->data.tokenIntVal = thisIDvar->varIntVal;
            }
            else if(thisIDvar->datatype == DATATYPE_NUM)
            {
                expressionToken->type = TOKEN_NUM;
                expressionToken->data.tokenNumVal = thisIDvar->varNumVal;
            }
            else if(thisIDvar->datatype == DATATYPE_STRING)
            {
                expressionToken->type = TOKEN_STR;
                expressionToken->data.tokenStringVal = thisIDvar->varStrVal;
            }
            else if(thisIDvar->datatype == DATATYPE_NIL)
            {
                expressionToken->type = TOKEN_NIL;
                //expressionToken->data.nilFlag = true;
            }
        }
        pushTokenStackTokens(stackOfVaruables, expressionToken);
    }
    
}

void buildTreeFromRuleSequence(ast_node *node, DynamicString *ruleSequenceString, StackTokens *stackOfVariables)
{
    printDynamicString(ruleSequenceString);
    if(ruleSequenceString->firstSymbol->symbol == '0')
    {
        node->nodeType = NODE_ID;
        token_t *token = popTokenStackTokens(stackOfVariables);
        switch (token->type)
        {
        case TOKEN_NUM:
            node->nodeData.doubleData = token->data.tokenNumVal;
            node->nodeType = NODE_NUM_ARG;
            break;
        case TOKEN_INT:
            node->nodeData.intData = token->data.tokenIntVal; 
            node->nodeData.nilFlag = false;   // HERE!!!!!
            node->nodeType = NODE_INT_ARG;
            break;
        case TOKEN_STR:
            //strcpy(node->nodeData.stringData, token->data.tokenStringVal);
            printf("aaaaaaaa: %s\n", token->data.tokenStringVal);
            node->nodeData.stringData = token->data.tokenStringVal;
            node->nodeType = NODE_STR_ARG;
            printf("aaaaaaaa: %s\n", node->nodeData.stringData);
            break;
        default:
            break;
        }

        ruleSequenceString->firstSymbol = ruleSequenceString->firstSymbol->nextSymbol;
        return;
    }
    else if(ruleSequenceString->firstSymbol->symbol == '1')
    {
        
        node->nodeType = NODE_STRLEN;
        ast_node *str = make_new_node();
        make_new_child(node, str);
        ruleSequenceString->firstSymbol = ruleSequenceString->firstSymbol->nextSymbol;
        buildTreeFromRuleSequence(node->childrenNodes[0], ruleSequenceString, stackOfVariables);
    }
    else
    {
        if(ruleSequenceString->firstSymbol->symbol == 'E')
        {
            ruleSequenceString->firstSymbol = ruleSequenceString->firstSymbol->nextSymbol;    
        }
        switch (ruleSequenceString->firstSymbol->symbol)
        {
        case '2':
            node->nodeType = NODE_MULT;
            break;
        case '3':
            node->nodeType = NODE_DIV;
            break;
        case '4':
            node->nodeType = NODE_INTDIV;
            break;
        case '5':
            node->nodeType = NODE_PLUS;
            break;
        case '6':
            node->nodeType = NODE_MINUS;
            break;
        case '7':
            node->nodeType = NODE_CONC;
            break;
        case '8':
            node->nodeType = NODE_LESS;
            break;
        case '9':
            node->nodeType = NODE_LEQ;
            break;
        case 'A':
            node->nodeType = NODE_GREATER;
            break;
        case 'B':
            node->nodeType = NODE_GEQ;
            break;
        case 'C':
            node->nodeType = NODE_EQUAL;
            break;
        case 'D':
            node->nodeType = NODE_NEQ;
            break;
        default:
            break;
        }
        ast_node *left = make_new_node();
        ast_node *right = make_new_node();
        make_new_child(node, left);
        make_new_child(node, right);
        ruleSequenceString->firstSymbol = ruleSequenceString->firstSymbol->nextSymbol;
        buildTreeFromRuleSequence(node->childrenNodes[1], ruleSequenceString, stackOfVariables);
        buildTreeFromRuleSequence(node->childrenNodes[0], ruleSequenceString, stackOfVariables);
    }

   
    
}

void expressionSemCheck(ast_node *leftOperand, ast_node *rightOperand, treeNodeType operatorType)
{
    switch (operatorType)
    {
    case NODE_MULT:
        if(leftOperand->nodeType == NODE_INT_ARG)
        {
            switch (rightOperand->nodeType)
            {
            case NODE_INT_ARG:
                return;
            case NODE_NUM_ARG:
                return;   
            default:
                fprintf(stderr,"NOTE: You're trying to mulitply INT by something wrong...\n");
                errorExit(SEMANTIC_IN_EXPRESSION_TYPES_ERR, 0); // TODO think about line
                break;
            }
        }
        else if(leftOperand->nodeType == NODE_NUM_ARG)
        {
            switch (rightOperand->nodeType)
            {
            case NODE_INT_ARG:
                return;
            case NODE_NUM_ARG:
                return;   
            default:
                fprintf(stderr,"NOTE: You're trying to mulitply NUM by something wrong...\n");
                errorExit(SEMANTIC_IN_EXPRESSION_TYPES_ERR, 0); // TODO think about line
                break;
            }
        }
        else if(leftOperand->nodeType == NODE_STR_ARG)
        {
            fprintf(stderr,"NOTE: You're trying to mulitply STR\n");
            errorExit(SEMANTIC_IN_EXPRESSION_TYPES_ERR, 0);
        }
        break;
    
    case NODE_DIV:
        if(leftOperand->nodeType == NODE_INT_ARG)
            {
                switch (rightOperand->nodeType)
                {
                case NODE_INT_ARG:
                    if(rightOperand->nodeData.zeroFlag == true) errorExit(ZERO_DIV_ERR, 0);
                    return;
                case NODE_NUM_ARG:
                    if(rightOperand->nodeData.zeroFlag == true) errorExit(ZERO_DIV_ERR, 0);
                    return;   
                default:
                    fprintf(stderr,"NOTE: You're trying to divide INT by something wrong...\n");
                    errorExit(SEMANTIC_IN_EXPRESSION_TYPES_ERR, 0); // TODO think about line
                    break;
                }
            }
            else if(leftOperand->nodeType == NODE_NUM_ARG)
            {
                switch (rightOperand->nodeType)
                {
                case NODE_INT_ARG:
                    if(rightOperand->nodeData.zeroFlag == true) errorExit(ZERO_DIV_ERR, 0);
                    return;
                case NODE_NUM_ARG:
                    if(rightOperand->nodeData.zeroFlag == true) errorExit(ZERO_DIV_ERR, 0);
                    return;   
                default:
                    fprintf(stderr,"NOTE: You're trying to divide NUM by something wrong...\n");
                    errorExit(SEMANTIC_IN_EXPRESSION_TYPES_ERR, 0); // TODO think about line
                    break;
                }
            }
            else if(leftOperand->nodeType == NODE_STR_ARG)
            {
                fprintf(stderr,"NOTE: You're trying to divide STR\n");
                errorExit(SEMANTIC_IN_EXPRESSION_TYPES_ERR, 0);
            }
            break;
        
    case NODE_MINUS:
    case NODE_PLUS:
        if(leftOperand->nodeType == NODE_INT_ARG)
        {
            switch (rightOperand->nodeType)
            {
            case NODE_INT_ARG:
                return;
            case NODE_NUM_ARG:
                return;   
            default:
                fprintf(stderr,"NOTE: You're trying to +/- INT with something wrong...\n");
                errorExit(SEMANTIC_IN_EXPRESSION_TYPES_ERR, 0); // TODO think about line
                break;
            }
        }
        else if(leftOperand->nodeType == NODE_NUM_ARG)
        {
            switch (rightOperand->nodeType)
            {
            case NODE_INT_ARG:
                return;
            case NODE_NUM_ARG:
                return;   
            default:
                fprintf(stderr,"NOTE: You're trying to +/- NUM with something wrong...\n");
                errorExit(SEMANTIC_IN_EXPRESSION_TYPES_ERR, 0); // TODO think about line
                break;
            }
        }
        else if(leftOperand->nodeType == NODE_STR_ARG)
        {
            fprintf(stderr,"NOTE: You're trying to +/- STR\n");
            errorExit(SEMANTIC_IN_EXPRESSION_TYPES_ERR, 0);
        }
        break;

    case NODE_CONC:
        if(leftOperand->nodeType == NODE_STR_ARG && rightOperand->nodeType == NODE_STR_ARG) return;
        else
        {
            fprintf(stderr, "NOTE: You're trying to concatenate something wrong\n");
            errorExit(SEMANTIC_IN_EXPRESSION_TYPES_ERR, 0);
        }
        break;
    case NODE_STRLEN:
        if(leftOperand->nodeType == NODE_STR_ARG) return;
        else
        {
            fprintf(stderr, "NOTE: You're trying to get length of something, what is not string\n");
            errorExit(SEMANTIC_IN_EXPRESSION_TYPES_ERR, 0);
        }
        break;

    case NODE_EQUAL:
    case NODE_NEQ:
        if(leftOperand->nodeType == NODE_INT_ARG || leftOperand->nodeType == NODE_NUM_ARG)
        {
            switch (rightOperand->nodeType)
            {
            case NODE_INT_ARG:
                return;
            case NODE_NUM_ARG:
                return;   
            default:
                fprintf(stderr,"NOTE: You're trying to compare INT/NUM with something wrong...\n");
                errorExit(SEMANTIC_IN_EXPRESSION_TYPES_ERR, 0); // TODO think about line
                break;
            }
        }
        if(leftOperand->nodeType == NODE_STR_ARG && rightOperand->nodeType == NODE_STR_ARG) return;
        else
        {
            fprintf(stderr, "NOTE : You're trying to compare string with something wrong\n");
            errorExit(SEMANTIC_IN_EXPRESSION_TYPES_ERR, 0);
        }
        break;
    case NODE_GEQ:
    case NODE_GREATER:
    case NODE_LEQ:
    case NODE_LESS:
        if(leftOperand->nodeType == NODE_INT_ARG || leftOperand->nodeType == NODE_NUM_ARG)
        {
            switch (rightOperand->nodeType)
            {
            case NODE_INT_ARG:
                return;
            case NODE_NUM_ARG:
                return;   
            default:
                fprintf(stderr,"NOTE: You're trying to compare INT/NUM with something wrong...\n");
                errorExit(SEMANTIC_IN_EXPRESSION_TYPES_ERR, 0); // TODO think about line
                break;
            }
        }
        break;
    default:
        break;
    }
}

void processNode(ast_node *node)
{
    switch (node->nodeType)
    {
    case NODE_STRLEN:
        //printf("test2: %s\n", node->childrenNodes[0]->nodeData.stringData);
        expressionSemCheck(node->childrenNodes[0], NULL, NODE_STRLEN);
        node->nodeData.intData = strlen(node->childrenNodes[0]->nodeData.stringData);
        //printf("test3\n");
        break;
    case NODE_PLUS:
        expressionSemCheck(node->childrenNodes[0], node->childrenNodes[1], NODE_PLUS);
        node->nodeData.intData = node->childrenNodes[0]->nodeData.intData + node->childrenNodes[1]->nodeData.intData;
        break;
    case NODE_MINUS:
        expressionSemCheck(node->childrenNodes[0], node->childrenNodes[1], NODE_MINUS);
        node->nodeData.intData = node->childrenNodes[0]->nodeData.intData - node->childrenNodes[1]->nodeData.intData;
        break;
    case NODE_MULT:
        expressionSemCheck(node->childrenNodes[0], node->childrenNodes[1], NODE_MULT);
        node->nodeData.intData = node->childrenNodes[0]->nodeData.intData * node->childrenNodes[1]->nodeData.intData;
        break;
    case NODE_DIV:
        expressionSemCheck(node->childrenNodes[0], node->childrenNodes[1], NODE_DIV);
        node->nodeData.intData = node->childrenNodes[0]->nodeData.intData / node->childrenNodes[1]->nodeData.intData;
        break;
    case NODE_INTDIV:
        expressionSemCheck(node->childrenNodes[0], node->childrenNodes[1], NODE_INTDIV);
        node->nodeData.intData = node->childrenNodes[0]->nodeData.intData / node->childrenNodes[1]->nodeData.intData;
        break;
    case NODE_CONC:
        expressionSemCheck(node->childrenNodes[0], node->childrenNodes[1], NODE_CONC);
        node->nodeData.stringData = strcat(node->childrenNodes[0]->nodeData.stringData, node->childrenNodes[2]->nodeData.stringData);
        break;
    case NODE_LESS:
        expressionSemCheck(node->childrenNodes[0], node->childrenNodes[1], NODE_LESS);
        if(node->childrenNodes[0]->nodeData.intData < node->childrenNodes[1]->nodeData.intData)
        {
            node->nodeData.intData = 1;
        }
        else
        {
            node->nodeData.nilFlag = true;
        }
        break;
    case NODE_LEQ:
        expressionSemCheck(node->childrenNodes[0], node->childrenNodes[1], NODE_LEQ);
        if(node->childrenNodes[0]->nodeData.intData <= node->childrenNodes[1]->nodeData.intData)
        {
            node->nodeData.intData = 1;
        }
        else
        {
            node->nodeData.nilFlag = true;
        }
        break;
    case NODE_GREATER:
        expressionSemCheck(node->childrenNodes[0], node->childrenNodes[1], NODE_GREATER);
        if(node->childrenNodes[0]->nodeData.intData > node->childrenNodes[1]->nodeData.intData)
        {
            node->nodeData.intData = 1;
        }
        else
        {
            node->nodeData.nilFlag = true;
        }
        break;
    case NODE_GEQ:
        expressionSemCheck(node->childrenNodes[0], node->childrenNodes[1], NODE_GEQ);
        if(node->childrenNodes[0]->nodeData.intData >= node->childrenNodes[1]->nodeData.intData)
        {
            node->nodeData.intData = 1;
        }
        else
        {
            node->nodeData.nilFlag = true;
        }
        break;
    case NODE_NEQ:
        expressionSemCheck(node->childrenNodes[0], node->childrenNodes[1], NODE_NEQ);
        if(node->childrenNodes[0]->nodeData.intData != node->childrenNodes[1]->nodeData.intData)
        {
            node->nodeData.intData = 1;
        }
        else
        {
            node->nodeData.nilFlag = true;
        }
        break;
    case NODE_EQUAL:
        expressionSemCheck(node->childrenNodes[0], node->childrenNodes[1], NODE_EQUAL);
        if(node->childrenNodes[0]->nodeData.intData == node->childrenNodes[1]->nodeData.intData)
        {
            node->nodeData.intData = 1;
        }
        else
        {
            node->nodeData.nilFlag = true;
        }
        break;
    default:
        break;
    }
    freeChildrenNodes(node);
}

void simplifyTheTree(ast_node *node)
{
    if(node->nodeType == NODE_ID || node->nodeType == NODE_INT_ARG || node->nodeType == NODE_NUM_ARG || node->nodeType == NODE_STR_ARG)
    {
        return;
    }
    else if(node->nodeType == NODE_STRLEN)
    {
        printf("test\n");
        simplifyTheTree(node->childrenNodes[0]);
        processNode(node);
    }
    else
    {
        simplifyTheTree(node->childrenNodes[0]);
        simplifyTheTree(node->childrenNodes[1]);
        processNode(node);
    }
}


ast_node *bottomUpAnalysis(htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)
{   
    StackTokens stackOfVariables;
    initStackTokens(&stackOfVariables);
    // create stack for none terminals
    NoneTerminalStack stackOfNoneTerminals;
    initNoneTerminalStack(&stackOfNoneTerminals);
    // create string for rule sequence
    DynamicString ruleSequenceString;
    DynamicStringInit(&ruleSequenceString);
    // token for expression elements
    token_t *expressionToken;
    // none terminal for stack of none terminals
    NoneTerminal nextNoneTerminal;
    NoneTerminal topNoneTerminal;

    // start process expression
    int ruleSequenceIsNotReady = 1; 
    // push $ none terminal to the stack of none terminals, it is bottom of the stack
    pushNoneTerminalElement(&stackOfNoneTerminals, NONE_TERMINAL_DOLLAR);
    topNoneTerminal = stackOfNoneTerminals.top->element;
    expressionToken = getToken(f, dynamicString, tokenStack);

    nextNoneTerminal = transformTokenToNoneTerminal(expressionToken->type);

    // main cycle that processes expression 
    while(ruleSequenceIsNotReady)
    {
        switch (precedenceTable[topNoneTerminal][nextNoneTerminal]) // check rule in precedece table 
        {
            case 0: // no rule
                
                if(topNoneTerminal == NONE_TERMINAL_DOLLAR && nextNoneTerminal == NONE_TERMINAL_DOLLAR)
                {
                  ruleSequenceIsNotReady = 0;  
                  ungetToken(expressionToken, tokenStack);
                  continue;
                }
                else
                {
                    errorExit(BAD_SYNTAX_ERR, expressionToken->line);
                }
                break;
            
            case 1: // reduce by the rule
                printf("Make reduce!!! Top is: %d\n", topNoneTerminal);
                reduceByTheRule(&topNoneTerminal, &stackOfNoneTerminals, &ruleSequenceString);
                continue;

            case 2: // shift 
                printf("Make shift!!! Top is: %d\n", topNoneTerminal);
                shiftElement(expressionToken, hashTableList, nextNoneTerminal, &stackOfVariables, &stackOfNoneTerminals);
                topNoneTerminal = stackOfNoneTerminals.top->element;
                break;

            case 3: // equal
                shiftElement(expressionToken, hashTableList, nextNoneTerminal, &stackOfVariables, &stackOfNoneTerminals);
                topNoneTerminal = stackOfNoneTerminals.top->element;
                break;    
            default:
                break;   
        }
        expressionToken = getToken(f, dynamicString, tokenStack);
        nextNoneTerminal = transformTokenToNoneTerminal(expressionToken->type);
    }
    // token.type = TOKEN_END

    // reversed rule sequence string 
    DynamicString reversedRuleSequenceString;
    DynamicStringInit(&reversedRuleSequenceString);
    DynamicStringReverse(&ruleSequenceString, &reversedRuleSequenceString);
    printDynamicString(&ruleSequenceString);
    DynamicStringDispose(&ruleSequenceString);
    printDynamicString(&reversedRuleSequenceString);
    // printf("%d\n", popTokenStackTokens(&stackOfVariables)->data.tokenIntVal);
    // printf("%d\n", popTokenStackTokens(&stackOfVariables)->data.tokenIntVal);
    //printf("%d\n", popTokenStackTokens(&stackOfVariables)->data.tokenIntVal);
    ast_node *expressionTree = make_new_node();
    expressionTree->nodeData.nilFlag = false;
    buildTreeFromRuleSequence(expressionTree, &reversedRuleSequenceString, &stackOfVariables);
    printAST(expressionTree);
    simplifyTheTree(expressionTree);

    
    
    if(expressionTree->nodeData.nilFlag)
    {
        expressionTree->nodeType = NODE_NIL_ARG;
    }
    else if(expressionTree->nodeData.zeroFlag)
    {
        expressionTree->nodeType = NODE_ZERO_ARG;
    }
    else if(expressionTree->nodeData.doubleData && (!expressionTree->nodeData.intData && !expressionTree->nodeData.stringData))
    {
        expressionTree->nodeType = NODE_NUM_ARG;
    }
    else if(expressionTree->nodeData.intData && (!expressionTree->nodeData.doubleData && !expressionTree->nodeData.stringData))
    {
        expressionTree->nodeType = NODE_INT_ARG;
    }
    else if(expressionTree->nodeData.stringData && (!expressionTree->nodeData.intData && !expressionTree->nodeData.doubleData))
    {
        expressionTree->nodeType = NODE_STR_ARG;
    }
    
    printAST(expressionTree);
    return expressionTree;
}


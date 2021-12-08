#include "bottom_up_analysis.h"
#include "string.h"
#include "stdio.h"

NoneTerminal transformTokenToNoneTerminal(tokenType_t type, token_t *token, htab_list_t *hashTableList, NoneTerminalStack *stackOfNoneTerminals)
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
            if(listSearch(hashTableList, token->data.tokenStringVal, FROM_FIRST)->type != TYPE_VARIABLE)
            {
                element = NONE_TERMINAL_DOLLAR;    
            }
            else if(stackOfNoneTerminals->top->element == NONE_TERMINAL_ID)
            {
                element = NONE_TERMINAL_DOLLAR;
            }
            else
            {
                element = NONE_TERMINAL_ID;    
            }
            break;
        case TOKEN_INT:
        case TOKEN_NUM:
        case TOKEN_STR:
        case TOKEN_NIL:
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

void shiftElement(token_t *expressionToken, NoneTerminal nextNoneTerminal, StackTokens *stackOfVaruables, NoneTerminalStack *stackOfNoneTerminals, htab_list_t *hashTableList, int flag)
{
    printf("FLAG %d\n", flag);
    pushNoneTerminalElement(stackOfNoneTerminals, nextNoneTerminal);
    if(expressionToken->type == TOKEN_STR || expressionToken->type == TOKEN_INT || expressionToken->type == TOKEN_NUM || expressionToken->type == TOKEN_ID || expressionToken->type == TOKEN_NIL)
    {
        if(expressionToken->type == TOKEN_ID)
        {
            printf("Check if\n");
            if(!listSearch(hashTableList, expressionToken->data.tokenStringVal, FROM_FIRST)) //check if there is this variable 
            {
                printf("Undefined variiable\n");
                exit(11);
            }
            else
            {
                printf("QQQQQQQQQQQQQQQQ\n");
                if(listSearch(hashTableList, expressionToken->data.tokenStringVal, FROM_FIRST)->type != TYPE_VARIABLE) // check if it's variable
                {
                    printf("NOT A VARIABLE\n");
                    exit(11);
                }
                else
                {
                    if(listSearch(hashTableList, expressionToken->data.tokenStringVal, FROM_FIRST)->defineFlag == 0) // check if variable is defined
                    {
                        printf("RRRRRRRRRRRRRRRRRRRR\n");
                        if(listSearch(hashTableList, expressionToken->data.tokenStringVal, FROM_FIRST)->declareFlag == 0)
                        {
                            printf("VARIABLE IS UNDEFINED\n");
                            exit(11);
                        }
                        else
                        {
                            expressionToken->type = TOKEN_NIL;
                            expressionToken->data.nilFlag = 0;
                        }
                    }
                    else // OK, it's variable 
                    {
                        printf("RERERERERRERERERER\n");
                        if(!flag)
                        {
                            printf("PRocess constant\n");
                            switch (listSearch(hashTableList, expressionToken->data.tokenStringVal, FROM_FIRST)->datatype)
                            {
                            case DATATYPE_INT:
                                expressionToken->type = TOKEN_INT;
                                expressionToken->data.tokenIntVal = listSearch(hashTableList, expressionToken->data.tokenStringVal, FROM_FIRST)->varIntVal;
                                break;
                            case DATATYPE_NUM:
                                expressionToken->type = TOKEN_NUM;
                                expressionToken->data.tokenNumVal = listSearch(hashTableList, expressionToken->data.tokenStringVal, FROM_FIRST)->varNumVal;
                                break;
                            case DATATYPE_STRING:
                                expressionToken->type = TOKEN_STR;
                                expressionToken->data.tokenStringVal = listSearch(hashTableList, expressionToken->data.tokenStringVal, FROM_FIRST)->varStrVal;
                                break;
                            default:
                                break;
                            }
                        }
                        
                    }
                }
            }
        }
        pushTokenStackTokens(stackOfVaruables, expressionToken);
    }
    
}

void buildTreeFromRuleSequence(ast_node *node, DynamicString *ruleSequenceString, StackTokens *stackOfVariables)
{
    if(ruleSequenceString->firstSymbol->symbol == '0')
    {
        token_t *token = popTokenStackTokens(stackOfVariables);
        switch (token->type)
        {
        case TOKEN_NUM:
            node->nodeType = NODE_NUM_ARG;
            node->nodeData.doubleData = token->data.tokenNumVal;
            break;
        case TOKEN_INT:
            node->nodeType = NODE_INT_ARG;
            node->nodeData.intData = token->data.tokenIntVal;   
            break;
        case TOKEN_STR:
            node->nodeType = NODE_STR_ARG;
            node->nodeData.stringData = token->data.tokenStringVal;
            break;
        case TOKEN_NIL: 
            printf("NODE: NIL\n");
            node->nodeType = NODE_NIL;
            node->nodeData.nilFlag = 0;
            break;
        case TOKEN_ID:
            printf("NODE: ID\n");
            node->nodeType = NODE_ID;
            node->nodeData.stringData = token->data.tokenStringVal;
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
        while(ruleSequenceString->firstSymbol->symbol == 'E')
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
    case NODE_STRLEN:
        if(leftOperand->nodeType != NODE_STR_ARG)
        {
            printf("ERROR: strlen, should be string argument");
            errorExit(SEMANTIC_IN_EXPRESSION_TYPES_ERR, 123);
        }
        break;
    case NODE_PLUS:
    case NODE_MINUS:
    case NODE_MULT:
    case NODE_DIV:
        if(leftOperand->nodeType == NODE_NIL || rightOperand->nodeType == NODE_NIL)
        {
            // TODO check error type 
            if(leftOperand->nodeData.nilFlag == 0)
            {
                errorExit(SEMANTIC_UNDEF_REDEF_ERR, 123);
            }
            else
            {
                errorExit(UNEXPECTED_NIL_ERR, 123);
            }
            
        }
        if (leftOperand->nodeType == NODE_STR_ARG || rightOperand->nodeType == NODE_STR_ARG || leftOperand->nodeType == NODE_NIL || rightOperand->nodeType == NODE_NIL)
        {
            printf("ERROR: +|-|/|*, has string as operand");
            errorExit(SEMANTIC_IN_EXPRESSION_TYPES_ERR, 123);
        }
        if(operatorType == NODE_DIV)
        {
            // if((rightOperand->nodeData.intData == 0 && rightOperand->nodeData.doubleData != 0) ||
            //    (rightOperand->nodeData.intData != 0 && rightOperand->nodeData.doubleData == 0) || 
            //    (rightOperand->nodeData.intData == 0 && rightOperand->nodeData.doubleData == 0))
            // {
            //     errorExit(ZERO_DIV_ERR, 123);
            // }
            if(rightOperand->nodeType == NODE_NUM_ARG && rightOperand->nodeData.doubleData == 0)
            {
                errorExit(ZERO_DIV_ERR, 123);
            }
            if(rightOperand->nodeType == NODE_INT_ARG && rightOperand->nodeData.intData == 0)
            {
                errorExit(ZERO_DIV_ERR, 123);
            }
        }
        break;
    case NODE_INTDIV:
        if (leftOperand->nodeType != NODE_INT_ARG || rightOperand->nodeType != NODE_INT_ARG)
        {
            printf("ERROR: //, should have two integer operands");
            errorExit(SEMANTIC_IN_EXPRESSION_TYPES_ERR, 123);
        }
        if((rightOperand->nodeData.intData == 0 && rightOperand->nodeData.doubleData != 0) ||
            (rightOperand->nodeData.intData != 0 && rightOperand->nodeData.doubleData == 0) || 
            (rightOperand->nodeData.intData == 0 && rightOperand->nodeData.doubleData == 0))
        {
            errorExit(ZERO_DIV_ERR, 123);
        }
        break;
    case NODE_CONC:
        if (leftOperand->nodeType != NODE_STR_ARG || rightOperand->nodeType != NODE_STR_ARG)
        {
            printf("ERROR: cancatenation, should has two string operands");
            errorExit(SEMANTIC_IN_EXPRESSION_TYPES_ERR, 123);
        }
        break;
    case NODE_LESS:
    case NODE_LEQ:
    case NODE_GREATER:
    case NODE_GEQ:
        if((leftOperand->nodeType == NODE_STR_ARG && rightOperand->nodeType != NODE_STR_ARG) || (leftOperand->nodeType != NODE_STR_ARG && rightOperand->nodeType == NODE_STR_ARG))
        {
            printf("ERROR: <|<=|>|>=, if one operand is string second must be string too");
            errorExit(SEMANTIC_IN_EXPRESSION_TYPES_ERR, 123);
        }
        if((leftOperand->nodeType == NODE_INT_ARG && rightOperand->nodeType == NODE_STR_ARG) || (leftOperand->nodeType == NODE_NUM_ARG && rightOperand->nodeType == NODE_STR_ARG))
        {
            printf("ERROR: <|<=|>|>=, if one operand is int/num second must be int/num too");
            errorExit(SEMANTIC_IN_EXPRESSION_TYPES_ERR, 123);
        }
        break;
    case NODE_EQUAL:
    case NODE_NEQ:
        // TODO: check one more time
        if(!(leftOperand->nodeType == NODE_NIL || rightOperand->nodeType == NODE_NIL))
        {
            if((leftOperand->nodeType == NODE_STR_ARG && rightOperand->nodeType != NODE_STR_ARG) || (leftOperand->nodeType != NODE_STR_ARG && rightOperand->nodeType == NODE_STR_ARG))
            {
                printf("ERROR: <|<=|>|>=, if one operand is string second must be string too");
                errorExit(SEMANTIC_IN_EXPRESSION_TYPES_ERR, 123);
            }
            else if((leftOperand->nodeType == NODE_INT_ARG && rightOperand->nodeType == NODE_STR_ARG) || (leftOperand->nodeType == NODE_NUM_ARG && rightOperand->nodeType == NODE_STR_ARG))
            {
                printf("ERROR: <|<=|>|>=, if one operand is int/num second must be int/num too");
                errorExit(SEMANTIC_IN_EXPRESSION_TYPES_ERR, 123);
            }
        }
        break;
    default:
        break;
    }
}

void processNode(ast_node *node)
{
    printf("PROCESS NODE\n");
    switch (node->nodeType)
    {
    case NODE_STRLEN:
        expressionSemCheck(node->childrenNodes[0], NULL, NODE_STRLEN);
        node->nodeData.intData = strlen(node->childrenNodes[0]->nodeData.stringData);
        node->nodeType = NODE_INT_ARG;
        break;
    case NODE_PLUS:
        expressionSemCheck(node->childrenNodes[0], node->childrenNodes[1], NODE_PLUS);
        if(node->childrenNodes[0]->nodeType == NODE_NUM_ARG)
        {
            if(node->childrenNodes[1]->nodeType == NODE_NUM_ARG)
            {
                node->nodeData.doubleData = node->childrenNodes[0]->nodeData.doubleData + node->childrenNodes[1]->nodeData.doubleData;
                node->nodeType = NODE_NUM_ARG;
            }
            else
            {
                node->nodeData.doubleData = node->childrenNodes[0]->nodeData.doubleData + node->childrenNodes[1]->nodeData.intData;
                node->nodeType = NODE_NUM_ARG;
            }
        }
        else
        {
            if(node->childrenNodes[1]->nodeType == NODE_NUM_ARG)
            {
                node->nodeData.doubleData = node->childrenNodes[0]->nodeData.intData + node->childrenNodes[1]->nodeData.doubleData;
                node->nodeType = NODE_NUM_ARG;
            }
            else
            {
                node->nodeData.intData = node->childrenNodes[0]->nodeData.intData + node->childrenNodes[1]->nodeData.intData;
                node->nodeType = NODE_INT_ARG;
            }
        }
        break;
    case NODE_MINUS:
        expressionSemCheck(node->childrenNodes[0], node->childrenNodes[1], NODE_MINUS);
        if(node->childrenNodes[0]->nodeType == NODE_NUM_ARG)
        {
            if(node->childrenNodes[1]->nodeType == NODE_NUM_ARG)
            {
                node->nodeData.doubleData = node->childrenNodes[0]->nodeData.doubleData - node->childrenNodes[1]->nodeData.doubleData;
                node->nodeType = NODE_NUM_ARG;
            }
            else
            {
                node->nodeData.doubleData = node->childrenNodes[0]->nodeData.doubleData - node->childrenNodes[1]->nodeData.intData;
                node->nodeType = NODE_NUM_ARG;
            }
        }
        else
        {
            if(node->childrenNodes[1]->nodeType == NODE_NUM_ARG)
            {
                node->nodeData.doubleData = node->childrenNodes[0]->nodeData.intData - node->childrenNodes[1]->nodeData.doubleData;
                node->nodeType = NODE_NUM_ARG;
            }
            else
            {
                node->nodeData.intData = node->childrenNodes[0]->nodeData.intData - node->childrenNodes[1]->nodeData.intData;
                node->nodeType = NODE_INT_ARG;
            }
        }
        break;
    case NODE_MULT:
        expressionSemCheck(node->childrenNodes[0], node->childrenNodes[1], NODE_MULT);
        if(node->childrenNodes[0]->nodeType == NODE_NUM_ARG)
        {
            if(node->childrenNodes[1]->nodeType == NODE_NUM_ARG)
            {
                node->nodeData.doubleData = node->childrenNodes[0]->nodeData.doubleData * node->childrenNodes[1]->nodeData.doubleData;
                node->nodeType = NODE_NUM_ARG;
            }
            else
            {
                node->nodeData.doubleData = node->childrenNodes[0]->nodeData.doubleData * node->childrenNodes[1]->nodeData.intData;
                node->nodeType = NODE_NUM_ARG;
            }
        }
        else
        {
            if(node->childrenNodes[1]->nodeType == NODE_NUM_ARG)
            {
                node->nodeData.doubleData = node->childrenNodes[0]->nodeData.intData * node->childrenNodes[1]->nodeData.doubleData;
                node->nodeType = NODE_NUM_ARG;
            }
            else
            {
                node->nodeData.intData = node->childrenNodes[0]->nodeData.intData * node->childrenNodes[1]->nodeData.intData;
                node->nodeType = NODE_INT_ARG;
            }
        }
        break;
    case NODE_DIV:
        expressionSemCheck(node->childrenNodes[0], node->childrenNodes[1], NODE_DIV);
        if(node->childrenNodes[0]->nodeType == NODE_NUM_ARG)
        {
            if(node->childrenNodes[1]->nodeType == NODE_NUM_ARG)
            {
                node->nodeData.doubleData = node->childrenNodes[0]->nodeData.doubleData / node->childrenNodes[1]->nodeData.doubleData;
                node->nodeType = NODE_NUM_ARG;
            }
            else
            {
                node->nodeData.doubleData = node->childrenNodes[0]->nodeData.doubleData / node->childrenNodes[1]->nodeData.intData;
                node->nodeType = NODE_NUM_ARG;
            }
        }
        else
        {
            if(node->childrenNodes[1]->nodeType == NODE_NUM_ARG)
            {
                node->nodeData.doubleData = node->childrenNodes[0]->nodeData.intData / node->childrenNodes[1]->nodeData.doubleData;
                node->nodeType = NODE_NUM_ARG;
            }
            else
            {
                node->nodeData.doubleData = (double)node->childrenNodes[0]->nodeData.intData / (double)node->childrenNodes[1]->nodeData.intData;
                node->nodeType = NODE_NUM_ARG;
            }
        }
        break;
    case NODE_INTDIV:
        expressionSemCheck(node->childrenNodes[0], node->childrenNodes[1], NODE_INTDIV);
        node->nodeData.intData = node->childrenNodes[0]->nodeData.intData / node->childrenNodes[1]->nodeData.intData;
        node->nodeType = NODE_INT_ARG;
        break;
    case NODE_CONC:
        expressionSemCheck(node->childrenNodes[0], node->childrenNodes[1], NODE_CONC);
        //node->nodeData.stringData = strcat(node->childrenNodes[0]->nodeData.stringData, node->childrenNodes[1]->nodeData.stringData);
        node->nodeData.stringData = malloc(sizeof(char)*(strlen(node->childrenNodes[0]->nodeData.stringData)+strlen(node->childrenNodes[1]->nodeData.stringData))); 
        strcat(node->nodeData.stringData, node->childrenNodes[0]->nodeData.stringData);
        strcat(node->nodeData.stringData, node->childrenNodes[1]->nodeData.stringData);
        node->nodeType = NODE_STR_ARG;
        break;
    case NODE_LESS:
        expressionSemCheck(node->childrenNodes[0], node->childrenNodes[1], NODE_LESS);
        if((node->childrenNodes[0]->nodeType == NODE_INT_ARG || node->childrenNodes[0]->nodeType == NODE_NUM_ARG) && (node->childrenNodes[1]->nodeType == NODE_INT_ARG || node->childrenNodes[1]->nodeType == NODE_NUM_ARG))
        {
            if(node->childrenNodes[0]->nodeType == NODE_NUM_ARG)
            {
                if(node->childrenNodes[1]->nodeType == NODE_NUM_ARG)
                {
                    if(node->childrenNodes[0]->nodeData.doubleData < node->childrenNodes[1]->nodeData.doubleData)
                    {
                        node->nodeType = NODE_TRUE;
                    }
                    else
                    {
                        node->nodeType = NODE_NIL;
                    }
                }
                else
                {
                    if(node->childrenNodes[0]->nodeData.doubleData < node->childrenNodes[1]->nodeData.intData)
                    {
                        node->nodeType = NODE_TRUE;
                    }
                    else
                    {
                        node->nodeType = NODE_NIL;
                    }
                }
            }
            else
            {
                if(node->childrenNodes[1]->nodeType == NODE_NUM_ARG)
                {
                    if(node->childrenNodes[0]->nodeData.intData < node->childrenNodes[1]->nodeData.doubleData)
                    {
                        node->nodeType = NODE_TRUE;
                    }
                    else
                    {
                        node->nodeType = NODE_NIL;
                    }
                }
                else
                {
                    if(node->childrenNodes[0]->nodeData.intData < node->childrenNodes[1]->nodeData.intData)
                    {
                        node->nodeType = NODE_TRUE;
                    }
                    else
                    {
                        node->nodeType = NODE_NIL;
                    }
                }
            }
        }
        if((node->childrenNodes[0]->nodeType == NODE_STR_ARG) && (node->childrenNodes[1]->nodeType == NODE_STR_ARG))
        {
            // TODO 
        }
        break;
    case NODE_LEQ:
        expressionSemCheck(node->childrenNodes[0], node->childrenNodes[1], NODE_LEQ);
        if((node->childrenNodes[0]->nodeType == NODE_INT_ARG || node->childrenNodes[0]->nodeType == NODE_NUM_ARG) && (node->childrenNodes[1]->nodeType == NODE_INT_ARG || node->childrenNodes[1]->nodeType == NODE_NUM_ARG))
        {
            if(node->childrenNodes[0]->nodeType == NODE_NUM_ARG)
            {
                if(node->childrenNodes[1]->nodeType == NODE_NUM_ARG)
                {
                    if(node->childrenNodes[0]->nodeData.doubleData <= node->childrenNodes[1]->nodeData.doubleData)
                    {
                        node->nodeType = NODE_TRUE;
                    }
                    else
                    {
                        node->nodeType = NODE_NIL;
                    }
                }
                else
                {
                    if(node->childrenNodes[0]->nodeData.doubleData <= node->childrenNodes[1]->nodeData.intData)
                    {
                        node->nodeType = NODE_TRUE;
                    }
                    else
                    {
                        node->nodeType = NODE_NIL;
                    }
                }
            }
            else
            {
                if(node->childrenNodes[1]->nodeType == NODE_NUM_ARG)
                {
                    if(node->childrenNodes[0]->nodeData.intData <= node->childrenNodes[1]->nodeData.doubleData)
                    {
                        node->nodeType = NODE_TRUE;
                    }
                    else
                    {
                        node->nodeType = NODE_NIL;
                    }
                }
                else
                {
                    if(node->childrenNodes[0]->nodeData.intData <= node->childrenNodes[1]->nodeData.intData)
                    {
                        node->nodeType = NODE_TRUE;
                    }
                    else
                    {
                        node->nodeType = NODE_NIL;
                    }
                }
            }
        }
        if((node->childrenNodes[0]->nodeType == NODE_STR_ARG) && (node->childrenNodes[1]->nodeType == NODE_STR_ARG))
        {
            // TODO 
        }
        break;
    case NODE_GREATER:
        expressionSemCheck(node->childrenNodes[0], node->childrenNodes[1], NODE_GREATER);
        if((node->childrenNodes[0]->nodeType == NODE_INT_ARG || node->childrenNodes[0]->nodeType == NODE_NUM_ARG) && (node->childrenNodes[1]->nodeType == NODE_INT_ARG || node->childrenNodes[1]->nodeType == NODE_NUM_ARG))
        {
            if(node->childrenNodes[0]->nodeType == NODE_NUM_ARG)
            {
                if(node->childrenNodes[1]->nodeType == NODE_NUM_ARG)
                {
                    if(node->childrenNodes[0]->nodeData.doubleData > node->childrenNodes[1]->nodeData.doubleData)
                    {
                        node->nodeType = NODE_TRUE;
                    }
                    else
                    {
                        node->nodeType = NODE_NIL;
                    }
                }
                else
                {
                    if(node->childrenNodes[0]->nodeData.doubleData > node->childrenNodes[1]->nodeData.intData)
                    {
                        node->nodeType = NODE_TRUE;
                    }
                    else
                    {
                        node->nodeType = NODE_NIL;
                    }
                }
            }
            else
            {
                if(node->childrenNodes[1]->nodeType == NODE_NUM_ARG)
                {
                    if(node->childrenNodes[0]->nodeData.intData > node->childrenNodes[1]->nodeData.doubleData)
                    {
                        node->nodeType = NODE_TRUE;
                    }
                    else
                    {
                        node->nodeType = NODE_NIL;
                    }
                }
                else
                {
                    if(node->childrenNodes[0]->nodeData.intData > node->childrenNodes[1]->nodeData.intData)
                    {
                        node->nodeType = NODE_TRUE;
                    }
                    else
                    {
                        node->nodeType = NODE_NIL;
                    }
                }
            }
        }
        if((node->childrenNodes[0]->nodeType == NODE_STR_ARG) && (node->childrenNodes[1]->nodeType == NODE_STR_ARG))
        {
            // TODO 
        }
        break;
    case NODE_GEQ:
        expressionSemCheck(node->childrenNodes[0], node->childrenNodes[1], NODE_GEQ);
        if((node->childrenNodes[0]->nodeType == NODE_INT_ARG || node->childrenNodes[0]->nodeType == NODE_NUM_ARG) && (node->childrenNodes[1]->nodeType == NODE_INT_ARG || node->childrenNodes[1]->nodeType == NODE_NUM_ARG))
        {
            if(node->childrenNodes[0]->nodeType == NODE_NUM_ARG)
            {
                if(node->childrenNodes[1]->nodeType == NODE_NUM_ARG)
                {
                    if(node->childrenNodes[0]->nodeData.doubleData >= node->childrenNodes[1]->nodeData.doubleData)
                    {
                        node->nodeType = NODE_TRUE;
                    }
                    else
                    {
                        node->nodeType = NODE_NIL;
                    }
                }
                else
                {
                    if(node->childrenNodes[0]->nodeData.doubleData >= node->childrenNodes[1]->nodeData.intData)
                    {
                        node->nodeType = NODE_TRUE;
                    }
                    else
                    {
                        node->nodeType = NODE_NIL;
                    }
                }
            }
            else
            {
                if(node->childrenNodes[1]->nodeType == NODE_NUM_ARG)
                {
                    if(node->childrenNodes[0]->nodeData.intData >= node->childrenNodes[1]->nodeData.doubleData)
                    {
                        node->nodeType = NODE_TRUE;
                    }
                    else
                    {
                        node->nodeType = NODE_NIL;
                    }
                }
                else
                {
                    if(node->childrenNodes[0]->nodeData.intData >= node->childrenNodes[1]->nodeData.intData)
                    {
                        node->nodeType = NODE_TRUE;
                    }
                    else
                    {
                        node->nodeType = NODE_NIL;
                    }
                }
            }
        }
        if((node->childrenNodes[0]->nodeType == NODE_STR_ARG) && (node->childrenNodes[1]->nodeType == NODE_STR_ARG))
        {
            // TODO 
        }
        break;
    case NODE_NEQ:
        expressionSemCheck(node->childrenNodes[0], node->childrenNodes[1], NODE_NEQ);
        if(node->childrenNodes[0]->nodeType == NODE_NIL)
        {
            if(node->childrenNodes[0]->nodeType != node->childrenNodes[1]->nodeType)
            {
                node->nodeType = NODE_TRUE;
            }
            else
            {
                node->nodeType = NODE_NIL;
            }
        }
        else if(node->childrenNodes[1]->nodeType == NODE_NIL)
        {
            if(node->childrenNodes[0]->nodeType != node->childrenNodes[1]->nodeType)
            {
                node->nodeType = NODE_TRUE;
            }
            else
            {
                node->nodeType = NODE_NIL;
            }
        }
        else if(node->childrenNodes[0]->nodeType == NODE_NUM_ARG)
        {
            if(node->childrenNodes[1]->nodeType == NODE_NUM_ARG)
            {
                if(node->childrenNodes[0]->nodeData.doubleData != node->childrenNodes[1]->nodeData.doubleData)
                {
                    node->nodeType = NODE_TRUE;
                }
                else
                {
                    node->nodeType = NODE_NIL;
                }
            }
            else
            {
                if(node->childrenNodes[0]->nodeData.doubleData != node->childrenNodes[1]->nodeData.intData)
                {
                    node->nodeType = NODE_TRUE;
                }
                else
                {
                    node->nodeType = NODE_NIL;
                }
            }
        }
        else if (node->childrenNodes[0]->nodeType == NODE_INT_ARG)
        {
            if(node->childrenNodes[1]->nodeType == NODE_NUM_ARG)
            {
                if(node->childrenNodes[0]->nodeData.intData != node->childrenNodes[1]->nodeData.doubleData)
                {
                    node->nodeType = NODE_TRUE;
                }
                else
                {
                    node->nodeType = NODE_NIL;
                }
            }
            else
            {
                if(node->childrenNodes[0]->nodeData.intData != node->childrenNodes[1]->nodeData.intData)
                {
                    node->nodeType = NODE_TRUE;
                }
                else
                {
                    node->nodeType = NODE_NIL;
                }
            }
        }
        else if(node->childrenNodes[0]->nodeType == NODE_STR_ARG)
        {
        
            if(strcmp(node->childrenNodes[0]->nodeData.stringData, node->childrenNodes[1]->nodeData.stringData) != 0)
            {
                node->nodeType = NODE_TRUE;
            }
            else
            {
                node->nodeType = NODE_NIL;
            }
        }
        break;
    case NODE_EQUAL:
        expressionSemCheck(node->childrenNodes[0], node->childrenNodes[1], NODE_EQUAL);
        if(node->childrenNodes[0]->nodeType == NODE_NIL)
        {
            if(node->childrenNodes[0]->nodeType == node->childrenNodes[1]->nodeType)
            {
                node->nodeType = NODE_TRUE;
            }
            else
            {
                node->nodeType = NODE_NIL;
            }
        }
        else if(node->childrenNodes[1]->nodeType == NODE_NIL)
        {
            if(node->childrenNodes[0]->nodeType == node->childrenNodes[1]->nodeType)
            {
                node->nodeType = NODE_TRUE;
            }
            else
            {
                node->nodeType = NODE_NIL;
            }
        }
        else if(node->childrenNodes[0]->nodeType == NODE_NUM_ARG)
        {
            if(node->childrenNodes[1]->nodeType == NODE_NUM_ARG)
            {
                if(node->childrenNodes[0]->nodeData.doubleData == node->childrenNodes[1]->nodeData.doubleData)
                {
                    node->nodeType = NODE_TRUE;
                }
                else
                {
                    node->nodeType = NODE_NIL;
                }
            }
            else
            {
                if(node->childrenNodes[0]->nodeData.doubleData == node->childrenNodes[1]->nodeData.intData)
                {
                    node->nodeType = NODE_TRUE;
                }
                else
                {
                    node->nodeType = NODE_NIL;
                }
            }
        }
        else if (node->childrenNodes[0]->nodeType == NODE_INT_ARG)
        {
            if(node->childrenNodes[1]->nodeType == NODE_NUM_ARG)
            {
                if(node->childrenNodes[0]->nodeData.intData == node->childrenNodes[1]->nodeData.doubleData)
                {
                    node->nodeType = NODE_TRUE;
                }
                else
                {
                    node->nodeType = NODE_NIL;
                }
            }
            else
            {
                if(node->childrenNodes[0]->nodeData.intData == node->childrenNodes[1]->nodeData.intData)
                {
                    node->nodeType = NODE_TRUE;
                }
                else
                {
                    node->nodeType = NODE_NIL;
                }
            }
        }
        else if(node->childrenNodes[0]->nodeType == NODE_STR_ARG)
        {
        
            if(strcmp(node->childrenNodes[0]->nodeData.stringData, node->childrenNodes[1]->nodeData.stringData) == 0)
            {
                node->nodeType = NODE_TRUE;
            }
            else
            {
                node->nodeType = NODE_NIL;
            }
        }
        break;
    default:
        break;
    }
    freeChildrenNodes(node);
}

void simplifyTheTree(ast_node *node)
{
    printf("TEST2\n");
    printf("NODE: %d\n", node->nodeType);
    if(node->nodeType == NODE_INT_ARG || node->nodeType == NODE_NUM_ARG || node->nodeType == NODE_STR_ARG || node->nodeType == NODE_NIL)
    {
        printf("NODE: %d", node->nodeType);
        return;
    }
    else if(node->nodeType == NODE_STRLEN)
    {
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

ast_node *bottomUpAnalysis(htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack, int flag)
{   
    // create stack for operands(IDs/consts)
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
    nextNoneTerminal = transformTokenToNoneTerminal(expressionToken->type, expressionToken, hashTableList, &stackOfNoneTerminals);

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
                    printf("error");
                    exit(1);
                }
                break;
            
            case 1: // reduce by the rule
                printf("Make reduce!!! Top is: %d\n", topNoneTerminal);
                reduceByTheRule(&topNoneTerminal, &stackOfNoneTerminals, &ruleSequenceString);
                continue;

            case 2: // shift 
                printf("Make shift!!! Top is: %d\n", topNoneTerminal);
                shiftElement(expressionToken, nextNoneTerminal, &stackOfVariables, &stackOfNoneTerminals, hashTableList, flag);
                topNoneTerminal = stackOfNoneTerminals.top->element;
                break;

            case 3: // equal
                shiftElement(expressionToken, nextNoneTerminal, &stackOfVariables, &stackOfNoneTerminals, hashTableList, flag);
                topNoneTerminal = stackOfNoneTerminals.top->element;
                break;    
            default:
                break;   
        }
        expressionToken = getToken(f, dynamicString, tokenStack);
        nextNoneTerminal = transformTokenToNoneTerminal(expressionToken->type, expressionToken, hashTableList, &stackOfNoneTerminals);
    }
    printf("Rule sequence is ready\n");
    // reversed rule sequence string 
    DynamicString reversedRuleSequenceString;
    DynamicStringInit(&reversedRuleSequenceString);
    DynamicStringReverse(&ruleSequenceString, &reversedRuleSequenceString);
    //printDynamicString(&ruleSequenceString);
    DynamicStringDispose(&ruleSequenceString);
    printDynamicString(&reversedRuleSequenceString);
    ast_node *expressionTree = make_new_node();
    printf("Be ready to build a tree\n");
    buildTreeFromRuleSequence(expressionTree, &reversedRuleSequenceString, &stackOfVariables);
    printf("TRee is built\n");
    printAST(expressionTree);
    
    if(flag)
    {
        return expressionTree;    
    }
    
    simplifyTheTree(expressionTree);
    printf("TEST\n");
    printAST(expressionTree);
    return expressionTree;
}


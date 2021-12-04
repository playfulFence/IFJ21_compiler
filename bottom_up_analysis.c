#include "bottom_up_analysis.h"
#include "string.h"
#include "stdio.h"

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

void shiftElement(token_t *expressionToken, NoneTerminal nextNoneTerminal, StackTokens *stackOfVaruables, NoneTerminalStack *stackOfNoneTerminals)
{
    pushNoneTerminalElement(stackOfNoneTerminals, nextNoneTerminal);
    if(expressionToken->type == TOKEN_STR || expressionToken->type == TOKEN_INT || expressionToken->type == TOKEN_NUM || expressionToken->type == TOKEN_ID)
    {
        if(expressionToken->type == TOKEN_ID)
        {
            // TODO find variable by id, change expression token type to INT/NUM/STR and set value INT/NUM/STR/NIL
            // maybe if variable has NIL value it's error, because you can't use undefined variable 
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
            break;
        case TOKEN_INT:
            node->nodeData.intData = token->data.tokenIntVal;   
            break;
        case TOKEN_STR:
            //strcpy(node->nodeData.stringData, token->data.tokenStringVal);
            printf("aaaaaaaa: %s\n", token->data.tokenStringVal);
            node->nodeData.stringData = token->data.tokenStringVal;
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
    // TODO check datatypes of operands for every type of operator
    // think about changing INT to NUM if it's legal for some of the operators(search information in google doc)
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
            node->nodeData.intData = 0;
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
            node->nodeData.intData = 0;
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
            node->nodeData.intData = 0;
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
            node->nodeData.intData = 0;
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
            node->nodeData.intData = 0;
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
            node->nodeData.intData = 0;
        }
        break;
    default:
        break;
    }
    freeChildrenNodes(node);
}

void simplifyTheTree(ast_node *node)
{
    if(node->nodeType == NODE_ID)
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
    // create new node, that will be return with all information about final value of expression 
    ast_node *returnExpressionNode = make_new_node();
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
                shiftElement(expressionToken, nextNoneTerminal, &stackOfVariables, &stackOfNoneTerminals);
                topNoneTerminal = stackOfNoneTerminals.top->element;
                break;

            case 3: // equal
                shiftElement(expressionToken, nextNoneTerminal, &stackOfVariables, &stackOfNoneTerminals);
                topNoneTerminal = stackOfNoneTerminals.top->element;
                break;    
            default:
                break;   
        }
        expressionToken = getToken(f, dynamicString, tokenStack);
        nextNoneTerminal = transformTokenToNoneTerminal(expressionToken->type);
    }
    
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
    buildTreeFromRuleSequence(expressionTree, &reversedRuleSequenceString, &stackOfVariables);
    printAST(expressionTree);
    simplifyTheTree(expressionTree);
    expressionTree->nodeType = NODE_ID;
    printAST(expressionTree);
    return returnExpressionNode;
}


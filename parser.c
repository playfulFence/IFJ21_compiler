#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "scanner.h"
#include "error.h"

void testScope(htab_list_t* hashTableList)
{
    // htab_list_t* testList = initList();
    // htab_t* testHtab = htab_init(43969);
    // htab_list_item_t* buffer = createItem(testHtab);
    // insertFirst(testList, buffer);

    // htab_lookup_add(testList->first->symtable, "keyword");
    // htab_lookup_add(testList->first->symtable, "yaYeblan");

    // listSearch(testList, "keyword", FROM_FIRST)->countOfArgs = 6;

    // htab_t* testShit = htab_init(43969);
    // htab_list_item_t* wtf = createItem(testShit);
    // insertFirst(testList, wtf);

    // htab_lookup_add(testList->first->symtable, "Maxon");
    // htab_lookup_add(testList->first->symtable, "keyword");
    // htab_lookup_add(testList->first->symtable, "Kiriyha");

    
    // listSearch(testList, "Maxon", FROM_FIRST)->countOfArgs = 29;

    // if(listSearch(testList, "Maxon", FROM_FIRST))
    // {
    //     if(listSearch(testList, "Maxon", FROM_FIRST)->countOfArgs == 29)
    //     {
    //         printf("Всё ок, заебал\n");
    //     }
    //     else printf("отъебись\n");
    // }
    // else printf("пососи\n");


    // listSearch(testList, "yaYeblan", FROM_FIRST)->countOfArgs = 228;
    

    // htab_lookup_add(testList->first->next->symtable, "Snova tuta");


    // htab_t* penis = htab_init(43969);
    // htab_list_item_t* penisItem = createItem(penis);
    // insertFirst(testList, penisItem);

    // htab_lookup_add(testList->first->symtable, "Я не дурак");
    // htab_lookup_add(testList->first->next->next->symtable, "Ну может хватит?");
    // htab_find(testList->first->next->next->symtable, "Ну может хватит?")->countOfArgs = 40;
    


    htab_list_item_t* tmpLooker = hashTableList->first;

    while(tmpLooker)
    {
        htab_for_each(tmpLooker->symtable, printWord);
        printf("\n\n");
        tmpLooker = tmpLooker->next;
    }


}


void processStatement(ast_node *funcDefNode, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack);

FILE* openFile(int argc, char** argv)
{
    FILE* f = (argc > 1) ? fopen(argv[1], "r") : stdin;
    return f;
}

int detectExpressionOrFunctionCall(tokenType_t tokenType, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)
{
    // TODO expression is only one variable!!!!
    token_t *nextToken;
    switch (tokenType)
    {
    case TOKEN_INT:
    case TOKEN_NUM:
    case TOKEN_STR:
    case TOKEN_NIL:
    case TOKEN_STRLEN:
    case TOKEN_L_BR:
    case TOKEN_R_BR:
        return 1; // return 1 means that it's expression 
        break;
    case TOKEN_ID:
        nextToken = getToken(f, dynamicString, tokenStack);
        if(nextToken->type == TOKEN_L_BR)
        {
            ungetToken(nextToken, tokenStack);
            return 2; // return 2 means that it's function call
        }
        else if(nextToken->type == TOKEN_PLUS || nextToken->type == TOKEN_MINUS || 
                nextToken->type == TOKEN_MULT || nextToken->type == TOKEN_S_BS ||
                nextToken->type == TOKEN_D_BS || nextToken->type == TOKEN_CONC)
        {
            ungetToken(nextToken, tokenStack);
            return 1; // return 1 means that it's expression 
        }
        else
        {
            ungetToken(nextToken, tokenStack);
            return 1; // return 0 means that it's error
        }
    default:
        return 0; // return 0 means that it's error
    }
}

// <statement> --> return <list_of_expressions>
void processReturnStatement(ast_node *returnNode, ast_node* funcDefNode, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)
{
    token_t *token = getToken(f, dynamicString, tokenStack);
    int returnCounter = 0;
    ast_node *expressionNode;
    token_t *nextToken;
    if(detectExpressionOrFunctionCall(token->type, f, dynamicString, tokenStack) == 1)
    {
        ungetToken(token, tokenStack);
        do
        {
            if(returnCounter > funcDefNode->hashTableItem->countOfReturns)          // return int x, string y, num z
            {
                printf("0\n");
                errorExit(SEMANTIC_PARAM_COUNT_ERR, token->line);                   // must return int, num, string
            }

            expressionNode = bottomUpAnalysis(hashTableList, f, dynamicString, tokenStack, 1); // TODO


            if(expressionNode->nodeType == NODE_STR_ARG && funcDefNode->hashTableItem->funcReturns[returnCounter]->datatype != DATATYPE_STRING)
            {
                printf("1\n");
                errorExit(SEMANTIC_PARAM_COUNT_ERR, token->line);
            }
            else if(expressionNode->nodeType == NODE_NUM_ARG &&                               // NUM can't be returned as INT, bit INT CAN be returned as NUM
                (funcDefNode->hashTableItem->funcReturns[returnCounter]->datatype == DATATYPE_INT))
            {
                printf("2\n");
                errorExit(SEMANTIC_PARAM_COUNT_ERR, token->line);
            }
           /* else if(funcDefNode->hashTableItem->funcReturns[returnCounter]->datatype == DATATYPE_NIL && expressionNode->nodeType = NODE_NIL)
            {
                errorExit(SEMANTIC_PARAM_COUNT_ERR, token->line);
            }*/
            


            make_new_child(returnNode, expressionNode);
            token = getToken(f, dynamicString, tokenStack);


            returnCounter++;
            if(token->type == TOKEN_COMMA)
            {
                nextToken = getToken(f, dynamicString, tokenStack);
                if(detectExpressionOrFunctionCall(nextToken->type, f, dynamicString, tokenStack) == 1)
                {
                    ungetToken(nextToken, tokenStack);
                }
                else
                {
                    fprintf(stderr, "NOTE %s - no expression after , token\n", __func__);
                    errorExit(BAD_SYNTAX_ERR, token->line);
                }
            }
            else
            {
                ungetToken(token, tokenStack);

                if(returnCounter < funcDefNode->hashTableItem->countOfReturns)
                {
                    printf("4\n");
                    errorExit(SEMANTIC_PARAM_COUNT_ERR, token->line);
                }
                return;
            }
            
        } while (token->type == TOKEN_COMMA);
    }
    
}

// <statement> --> while expression do <list_of_statements> end
void processWhileStatement(ast_node *whileNode, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)
{
    ast_node *whileConditionNode = make_new_node();
     
    whileConditionNode->nodeType = NODE_WHILE_CONDITION;
    make_new_child(whileNode, whileConditionNode);
    ast_node *conditionNode = bottomUpAnalysis(hashTableList, f, dynamicString, tokenStack, 1);
    make_new_child(whileConditionNode, conditionNode);
    token_t *token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_DO)
    {
        printf("NOTE %s - no \"do\" token\n", __func__);
        errorExit(BAD_SYNTAX_ERR, token->line);
    }
    ast_node *whileDoNode = make_new_node();
    whileDoNode->nodeType = NODE_WHILE_DO;
    make_new_child(whileNode, whileDoNode);
    // get next token, if its not END process list of statements
    token = getToken(f, dynamicString, tokenStack);

    htab_t* levelWhileTable = htab_init(43969);               /* To correctly simulate multilevel structure of scope hierarchy we need to */
    htab_list_item_t* newItem = createItem(levelWhileTable);      /* make next element in our htab-list */
    insertFirst(hashTableList, newItem);
    while(token->type != TOKEN_END)
    {
        ungetToken(token, tokenStack);
        processStatement(whileDoNode, hashTableList, f, dynamicString, tokenStack);
        token = getToken(f, dynamicString, tokenStack);
    }

    removeFirst(hashTableList); // whole "while"-statement has been executed -> remove it from scope

}

// <statement> --> if expression then <list_of_statements> else <list_of_statements>  end
void processIfStatement(ast_node *ifNode, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)
{
    ast_node *ifConditionNode = make_new_node();
    ifConditionNode->nodeType = NODE_IF_CONDITION;
    make_new_child(ifNode, ifConditionNode);
    ast_node *conditionNode = bottomUpAnalysis(hashTableList, f, dynamicString, tokenStack, 1);
    make_new_child(ifConditionNode, conditionNode);
    
    token_t *token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_THEN)
    {
        fprintf(stderr, "NOTE %s - no \"then\" token\n", __func__);
        errorExit(BAD_SYNTAX_ERR, token->line);
    }
    ast_node *thenNode = make_new_node();
    thenNode->nodeType = NODE_IF_THEN;
    make_new_child(ifNode, thenNode);
    
    // get next token, if its not END process list of statements
    token = getToken(f, dynamicString, tokenStack);

    htab_t* levelIfThenTable = htab_init(43969);               /* To correctly simulate multilevel structure of scope hierarchy we need to */
    htab_list_item_t* newItem = createItem(levelIfThenTable);      /* make next element in our htab-list */
    insertFirst(hashTableList, newItem);
    
    while(token->type != TOKEN_ELSE)
    {
        printf("CHECK THEN\n");
        ungetToken(token, tokenStack);
        processStatement(thenNode, hashTableList, f, dynamicString, tokenStack);
        printf("CHECK THEN AFTER\n");
        token = getToken(f, dynamicString, tokenStack);
    }
    removeFirst(hashTableList); // removed "then"-statement table from scope structure


    ast_node *elseNode = make_new_node();
    elseNode->nodeType = NODE_IF_ELSE;
    make_new_child(ifNode, elseNode);
    token = getToken(f, dynamicString, tokenStack);

    htab_t* levelIfElseTable = htab_init(43969);               /* To correctly simulate multilevel structure of scope hierarchy we need to */
    htab_list_item_t* oneMoreNewItem = createItem(levelIfElseTable);      /* make next element in our htab-list */
    insertFirst(hashTableList, oneMoreNewItem);
    while(token->type != TOKEN_END)
    {
        ungetToken(token, tokenStack);
        processStatement(elseNode, hashTableList, f, dynamicString, tokenStack);
        token = getToken(f, dynamicString, tokenStack);
    }

    removeFirst(hashTableList); // whole "if"-statement has been processed, now we can clear "else"-statement table from scope structure
}

// <statement> --> id (<list_of_arguments>)
// TODO check variables compatibility
void processFuncCall(ast_node *funcCallNode, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)
{   
    // get token, should be ID
    
    token_t *token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_ID)
    {
        fprintf(stderr, "NOTE %s - no ID token\n", __func__);
        errorExit(BAD_SYNTAX_ERR, token->line);
    }
    
    if(!listSearch(hashTableList, token->data.tokenStringVal, FROM_FIRST))
    {
        fprintf(stderr, "NOTE: Function with name %s wasn't found\n", token->data.tokenStringVal);
        errorExit(SEMANTIC_UNDEF_REDEF_ERR, token->line);
    }
    else if(listSearch(hashTableList, token->data.tokenStringVal, FROM_FIRST)->type != TYPE_FUNC)
    {
        fprintf(stderr,"NOTE: You want to call something, what isn't function\n");
        errorExit(SEMANTIC_ANOTHER_ERR, token->line);
    }


    funcCallNode->nodeData.stringData = token->data.tokenStringVal;
    funcCallNode->hashTableItem = copyDataFuncCall(listSearch(hashTableList, token->data.tokenStringVal, FROM_FIRST), funcCallNode->hashTableItem);



    // get token, should be (
    token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_L_BR)
    {
        fprintf(stderr,"NOTE: processFuncCall - no \"(\" token\n");
        errorExit(BAD_SYNTAX_ERR, token->line);
    }

    token = getToken(f, dynamicString, tokenStack);


    int countActualArgs = 0; 
    // get token, if it's not ) token, process arguments
    while (token->type != TOKEN_R_BR)
    {
        if(countActualArgs > funcCallNode->hashTableItem->countOfArgs)
        {
            fprintf(stderr, "NOTE: In function %s\n",token->data.tokenStringVal);
            errorExit(SEMANTIC_PARAM_COUNT_ERR, token->line);
        }

        printf("%d ==== token type \n", funcCallNode->hashTableItem->funcArgs[countActualArgs]->datatype);
        switch (token->type)
        {
        case TOKEN_INT:
            if(funcCallNode->hashTableItem->funcArgs[countActualArgs]->datatype == DATATYPE_STRING)
            {   
                printf("ЭТО?\n");
                fprintf(stderr, "NOTE: Argument number %d in function %s\n", countActualArgs, token->data.tokenStringVal);
                errorExit(SEMANTIC_PARAM_COUNT_ERR, token->line);
            }
            //TODO check compatibility of INT and NUM
            funcCallNode->hashTableItem->funcArgs[countActualArgs]->varIntVal = token->data.tokenIntVal;
            break;
        case TOKEN_NUM:
        if(funcCallNode->hashTableItem->funcArgs[countActualArgs]->datatype != DATATYPE_NUM)
            {
                fprintf(stderr, "NOTE: Argument number %d in function %s\n", countActualArgs, token->data.tokenStringVal);
                errorExit(SEMANTIC_PARAM_COUNT_ERR, token->line);
            }
            
            funcCallNode->hashTableItem->funcArgs[countActualArgs]->varNumVal = token->data.tokenNumVal;
            break;
        case TOKEN_STR:
        if(funcCallNode->hashTableItem->funcArgs[countActualArgs]->datatype != DATATYPE_STRING)
            {
                fprintf(stderr, "NOTE: Argument number %d in function %s\n", countActualArgs, token->data.tokenStringVal);
                errorExit(SEMANTIC_PARAM_COUNT_ERR, token->line);
            }

            funcCallNode->hashTableItem->funcArgs[countActualArgs]->varStrVal = token->data.tokenStringVal;
            break;
        case TOKEN_ID:
            switch (token->type)
            {
            case TYPE_INT:
                if(funcCallNode->hashTableItem->funcArgs[countActualArgs]->datatype == TYPE_STRING)
                {
                    fprintf(stderr, "NOTE: Argument number %d function %s\n", countActualArgs, token->data.tokenStringVal);
                    errorExit(SEMANTIC_PARAM_COUNT_ERR, token->line);
                }
                // TODO int-num compatibility
                funcCallNode->hashTableItem->funcArgs[countActualArgs]->varIntVal = token->data.tokenIntVal;
                break;
            case TYPE_NUM:
                if(funcCallNode->hashTableItem->funcArgs[countActualArgs]->datatype != TYPE_NUM)
                {
                    fprintf(stderr, "NOTE: Argument number %d function %s\n", countActualArgs, token->data.tokenStringVal);
                    errorExit(SEMANTIC_PARAM_COUNT_ERR, token->line);
                }

                funcCallNode->hashTableItem->funcArgs[countActualArgs]->varNumVal = token->data.tokenNumVal;
                break;
            case TYPE_STRING:
                if(funcCallNode->hashTableItem->funcArgs[countActualArgs]->datatype != TYPE_STRING)
                {
                    fprintf(stderr, "NOTE: Argument number %d function %s\n", countActualArgs, token->data.tokenStringVal);
                    errorExit(SEMANTIC_PARAM_COUNT_ERR, token->line);;
                }
                
                funcCallNode->hashTableItem->funcArgs[countActualArgs]->varStrVal = token->data.tokenStringVal;
                break;
            case TYPE_NIL: //TODO
                break;
            default:
                break;
            }
            break;
        case TOKEN_COMMA:
            token = getToken(f, dynamicString, tokenStack);
            continue;
        default:
            fprintf(stderr,"ERROR processFuncCall - wrong argument token\n");
            errorExit(SEMANTIC_ANOTHER_ERR, token->line);
            break;
        }
        
        token = getToken(f, dynamicString, tokenStack);
        countActualArgs++;
    }
    if(countActualArgs < funcCallNode->hashTableItem->countOfArgs)
        {
            fprintf(stderr, "NOTE: In function %s\n",token->data.tokenStringVal);
            errorExit(SEMANTIC_PARAM_COUNT_ERR, token->line);
        }

    
}

// <statement> --> id = <expression_or_func_call>
void processSingleAssignment(ast_node *singleAssignNode, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)
{
    printf("CHECK THEN IN ASSIGNMENT\n");
    token_t *token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_ID)
    {
        fprintf(stderr, "NOTE: %s - no ID token\n", __func__);
        errorExit(BAD_SYNTAX_ERR, token->line);
    }
    ast_node *idNode = make_new_node();
    idNode->nodeType = NODE_ID;
    idNode->nodeData.stringData = token->data.tokenStringVal;
    
    if(!listSearch(hashTableList, token->data.tokenStringVal, FROM_FIRST))
    {
        errorExit(SEMANTIC_UNDEF_REDEF_ERR, token->line);
    }
    else
    {
        if(htab_find(hashTableList->first->symtable, token->data.tokenStringVal)) // if variable is declared on CURRENT level
        {
            idNode->hashTableItem = htab_find(hashTableList->first->symtable, token->data.tokenStringVal);
        }
        else
        {
            idNode->hashTableItem = listSearch(hashTableList, token->data.tokenStringVal, FROM_SECOND);
        }
    
    }
    
    make_new_child(singleAssignNode, idNode);
    token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_ASSIGNMENT)
    {
        fprintf(stderr, "NOTE processSingleAssignment - no = token\n");
        errorExit(BAD_SYNTAX_ERR, token->line);
    }
    // get next token, should be start of expression or start of function call 
    token = getToken(f, dynamicString, tokenStack);
    ast_node *expressionsNode;
    ast_node *expressionNode;
    ast_node *funcCallNode;
    int counterOfValues = 0;
    token_t *nextToken;
    
    switch (detectExpressionOrFunctionCall(token->type, f, dynamicString, tokenStack))
    {
    case 1: // expression
        // make expressions node that will be parent node for all IDs in assignment
        //expressionsNode = make_new_node();
        //expressionsNode->nodeType = NODE_VALUES;
        //make_new_child(singleAssignNode, expressionsNode);
        
        ungetToken(token, tokenStack);
        do
        {
            expressionNode = bottomUpAnalysis(hashTableList, f, dynamicString, tokenStack, 0); // TODO
            //make_new_child(expressionsNode, expressionNode);
            if(counterOfValues < singleAssignNode->childrenCounter)
            {
                switch (expressionNode->nodeType)
                {
                case NODE_INT_ARG:
                    if(singleAssignNode->childrenNodes[counterOfValues]->hashTableItem->datatype != DATATYPE_INT)
                    {
                        errorExit(SEMANTIC_ASSIGN_ERR, 122);
                    }
                    singleAssignNode->childrenNodes[counterOfValues]->nodeData.intData = expressionNode->nodeData.intData;
                    singleAssignNode->childrenNodes[counterOfValues]->hashTableItem->varIntVal = singleAssignNode->childrenNodes[counterOfValues]->nodeData.intData;
                    singleAssignNode->childrenNodes[counterOfValues]->hashTableItem->defineFlag = 1;
                    idNode->nodeType = NODE_INT_ARG;
                    break;
                case NODE_NUM_ARG:
                    if(singleAssignNode->childrenNodes[counterOfValues]->hashTableItem->datatype != DATATYPE_NUM)
                    {
                        errorExit(SEMANTIC_ASSIGN_ERR, 122);
                    }
                    singleAssignNode->childrenNodes[counterOfValues]->nodeData.doubleData = expressionNode->nodeData.doubleData;
                    singleAssignNode->childrenNodes[counterOfValues]->hashTableItem->varNumVal = singleAssignNode->childrenNodes[counterOfValues]->nodeData.doubleData;
                    singleAssignNode->childrenNodes[counterOfValues]->hashTableItem->defineFlag = 1;
                    idNode->nodeType = NODE_NUM_ARG;
                    break;
                case NODE_STR_ARG:
                    if(singleAssignNode->childrenNodes[counterOfValues]->hashTableItem->datatype != DATATYPE_STRING)
                    {
                        errorExit(SEMANTIC_ASSIGN_ERR, 122);
                    }
                    singleAssignNode->childrenNodes[counterOfValues]->nodeData.stringData = expressionNode->nodeData.stringData;
                    singleAssignNode->childrenNodes[counterOfValues]->hashTableItem->varStrVal = malloc(sizeof(char) * strlen(singleAssignNode->childrenNodes[counterOfValues]->nodeData.stringData));
                    singleAssignNode->childrenNodes[counterOfValues]->hashTableItem->defineFlag = 1;
                    strcpy(singleAssignNode->childrenNodes[counterOfValues]->hashTableItem->varStrVal, singleAssignNode->childrenNodes[counterOfValues]->nodeData.stringData);
                    idNode->nodeType = NODE_STR_ARG;
                    break;
                case NODE_TRUE:
                    printf("ASSIGN WITH TRUE\n");
                    errorExit(SEMANTIC_IN_EXPRESSION_TYPES_ERR, 1243);
                case NODE_NIL:
                    // TODO NOT SURE HOW IT SHOULD BE WITH ASSIGN NIL
                    singleAssignNode->childrenNodes[counterOfValues]->hashTableItem->type = TYPE_VARIABLE;
                    singleAssignNode->childrenNodes[counterOfValues]->hashTableItem->declareFlag = true;    // means that value is NIL!!!!
                    singleAssignNode->childrenNodes[counterOfValues]->hashTableItem->defineFlag = false;        // p.s. we're so smart to make new flag
                    idNode->nodeType = NODE_NIL;
                    return;
                default:
                    break;
                }
            }
            else
            {
                printf("DIFFERENT AMMOUNT OF IDS AND VALUES\n");
                // TODO CHECK ERROR TYPE 
                errorExit(SEMANTIC_ASSIGN_ERR,123);
            }
            token = getToken(f, dynamicString, tokenStack);
            if(token->type == TOKEN_COMMA)
            {
                nextToken = getToken(f, dynamicString, tokenStack);
                if(detectExpressionOrFunctionCall(nextToken->type, f, dynamicString, tokenStack) == 1)
                {
                    ungetToken(nextToken, tokenStack);
                    counterOfValues++;
                }
                else
                {
                    fprintf(stderr, "NOTE processMultipleAssignment - no expression after , token\n");
                    errorExit(BAD_SYNTAX_ERR, token->line);
                }
            }
            else
            {
                // TODO CHECK IF IT'S TIGHT PLACE TO UNGET TOKEN
                ungetToken(token, tokenStack);
                return;
            } 
            
        } while (token->type == TOKEN_COMMA);
        
        break;
    case 2: // function call 
        // TODO check types and ammount of returns 
        funcCallNode = make_new_node();
        funcCallNode->nodeType = NODE_FUNC_CALL;
        make_new_child(singleAssignNode, funcCallNode);  // TODO CHECK VOID
        ungetToken(token, tokenStack);
        processFuncCall(funcCallNode, hashTableList, f, dynamicString, tokenStack);
        
        if(singleAssignNode->childrenNodes[0]->hashTableItem->datatype == DATATYPE_INT)
        {
            if(funcCallNode->hashTableItem->funcReturns[0]->datatype != DATATYPE_INT)
            {
                errorExit(SEMANTIC_ASSIGN_ERR, 123);
            }
        }
        else if(singleAssignNode->childrenNodes[0]->hashTableItem->datatype == DATATYPE_NUM)
        {
            if(funcCallNode->hashTableItem->funcReturns[0]->datatype == DATATYPE_STRING)
            {
                errorExit(SEMANTIC_ASSIGN_ERR, 321);
            }
        }
        else if(singleAssignNode->childrenNodes[0]->hashTableItem->datatype == DATATYPE_STRING)
        {
            if(funcCallNode->hashTableItem->funcReturns[0]->datatype != DATATYPE_STRING)
            {
                errorExit(SEMANTIC_ASSIGN_ERR, 228);
            }
        }
        break;
    default:
        fprintf(stderr, "NOTE processVariableDefStatement - error with assignment\n");
        errorExit(SEMANTIC_ANOTHER_ERR, token->line);
        break;
    }

}

// <statement> --> id <more_ids> = <expression_or_func_call>
void processMultipleAssignment(ast_node *multAssignNode, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)
{   
    // TODO CHECK ONE MORE TIME !!!


    // make IDs node that will be parent node for all IDs in assignment
    ast_node *idsNode = make_new_node();
    idsNode->nodeType = NODE_IDS;
    make_new_child(multAssignNode, idsNode);
    // process IDs 
    token_t *token = getToken(f, dynamicString, tokenStack);
    while(token->type != TOKEN_ASSIGNMENT)
    {
        if(token->type != TOKEN_ID)
        {
            fprintf(stderr, "NOTE: %s - no ID token\n", __func__);
            errorExit(BAD_SYNTAX_ERR, token->line);
        }


        ast_node *idNode = make_new_node();
        idNode->nodeType = NODE_ID;
        idNode->nodeData.stringData = token->data.tokenStringVal;
        if(!listSearch(hashTableList, token->data.tokenStringVal, FROM_FIRST))
        {
            errorExit(SEMANTIC_UNDEF_REDEF_ERR, token->line);
        }
        else
        {
            if(htab_find(hashTableList->first->symtable, token->data.tokenStringVal)) // if variable is declared on CURRENT level
            {
                idNode->hashTableItem = htab_find(hashTableList->first->symtable, token->data.tokenStringVal);
            }
            else
            {
                idNode->hashTableItem = listSearch(hashTableList, token->data.tokenStringVal, FROM_SECOND);
            }
        
        }


        token = getToken(f, dynamicString, tokenStack);
        if(token->type != TOKEN_COMMA && token->type != TOKEN_ASSIGNMENT)
        {
            fprintf(stderr, "ERROR processMultipleAssignment - no , token\n");    
            errorExit(BAD_SYNTAX_ERR, token->line);
        }
        else if(token->type == TOKEN_ASSIGNMENT)
        {
            make_new_child(idsNode, idNode);
            break;    
        }
        make_new_child(idsNode, idNode);
        token = getToken(f, dynamicString, tokenStack);
    }

    // get next token, should be start of expression or start of function call 
    token = getToken(f, dynamicString, tokenStack);
    ast_node *expressionNode;
    ast_node *funcCallNode;
    int counterOfValues = 0;
    token_t *nextToken;
    switch (detectExpressionOrFunctionCall(token->type, f, dynamicString, tokenStack))
    {
    case 1: // expression
        ungetToken(token, tokenStack);
        // TODO think about func in expressions list
        do
        {
            expressionNode = bottomUpAnalysis(hashTableList, f, dynamicString, tokenStack, 0); // TODO

            // TODO CHECK IF THERE ARE LESS VALUES THAN IDS
            if(counterOfValues < idsNode->childrenCounter)
            {
                switch (expressionNode->nodeType)
                {
                case NODE_INT_ARG:
                    if(idsNode->childrenNodes[counterOfValues]->hashTableItem->datatype != DATATYPE_INT)
                    {
                        errorExit(SEMANTIC_ASSIGN_ERR, 122);
                    }
                    idsNode->childrenNodes[counterOfValues]->nodeData.intData = expressionNode->nodeData.intData;
                    idsNode->childrenNodes[counterOfValues]->hashTableItem->varIntVal = idsNode->childrenNodes[counterOfValues]->nodeData.intData;
                    idsNode->childrenNodes[counterOfValues]->hashTableItem->defineFlag = 1;
                    break;
                case NODE_NUM_ARG:
                    if(idsNode->childrenNodes[counterOfValues]->hashTableItem->datatype != DATATYPE_NUM)
                    {
                        errorExit(SEMANTIC_ASSIGN_ERR, 122);
                    }
                    idsNode->childrenNodes[counterOfValues]->nodeData.doubleData = expressionNode->nodeData.doubleData;
                    idsNode->childrenNodes[counterOfValues]->hashTableItem->varNumVal = idsNode->childrenNodes[counterOfValues]->nodeData.doubleData;
                    idsNode->childrenNodes[counterOfValues]->hashTableItem->defineFlag = 1;
                    break;
                case NODE_STR_ARG:
                    if(idsNode->childrenNodes[counterOfValues]->hashTableItem->datatype != DATATYPE_STRING)
                    {
                        errorExit(SEMANTIC_ASSIGN_ERR, 122);
                    }
                    idsNode->childrenNodes[counterOfValues]->nodeData.stringData = expressionNode->nodeData.stringData;
                    idsNode->childrenNodes[counterOfValues]->hashTableItem->varStrVal = malloc(sizeof(char) * strlen(idsNode->childrenNodes[counterOfValues]->nodeData.stringData));
                    strcpy(idsNode->childrenNodes[counterOfValues]->hashTableItem->varStrVal, idsNode->childrenNodes[counterOfValues]->nodeData.stringData);
                    idsNode->childrenNodes[counterOfValues]->hashTableItem->defineFlag = 1;
                    break;
                case NODE_TRUE:
                    printf("ASSIGN WITH TRUE\n");
                    errorExit(SEMANTIC_IN_EXPRESSION_TYPES_ERR, 1243);
                case NODE_NIL:
                    // TODO NOT SURE HOW IT SHOULD BE WITH ASSIGN NIL
                    idsNode->childrenNodes[counterOfValues]->hashTableItem->type = TYPE_VARIABLE;
                    idsNode->childrenNodes[counterOfValues]->hashTableItem->declareFlag = true;    // means that value is NIL!!!!
                    idsNode->childrenNodes[counterOfValues]->hashTableItem->defineFlag = false;        // p.s. we're so smart to make new flag
                    return;
                default:
                    break;
                }
            }
            else
            {
                printf("DIFFERENT AMMOUNT OF IDS AND VALUES\n");
                // TODO CHECK ERROR TYPE 
                errorExit(SEMANTIC_ASSIGN_ERR,123);
            }
            token = getToken(f, dynamicString, tokenStack);
            if(token->type == TOKEN_COMMA)
            {
                nextToken = getToken(f, dynamicString, tokenStack);
                if(detectExpressionOrFunctionCall(nextToken->type, f, dynamicString, tokenStack) == 1)
                {  // if expression 

                    ungetToken(nextToken, tokenStack);
                    counterOfValues++;
                    continue;
                }
                else // if whatever except of expression
                {
                    fprintf(stderr, "NOTE processMultipleAssignment - no expression after , token\n");
                    errorExit(BAD_SYNTAX_ERR, token->line);
                }
            }
            else
            {
                // TODO check 
                ungetToken(token, tokenStack);
                return;
            }
        } while (token->type == TOKEN_COMMA);
        
        break;
    case 2: // function call 
        // TODO MAKE THIS 
        funcCallNode = make_new_node();
        funcCallNode->nodeType = NODE_FUNC_CALL;
        make_new_child(multAssignNode, funcCallNode);
        ungetToken(token, tokenStack);
        processFuncCall(funcCallNode, hashTableList, f, dynamicString, tokenStack);

        for(int i = 0; i < idsNode->childrenCounter; i++)
            if(idsNode->childrenNodes[i]->hashTableItem->datatype == DATATYPE_INT)
            {
                if(funcCallNode->hashTableItem->funcReturns[i]->datatype != DATATYPE_INT)
                {
                    errorExit(SEMANTIC_ASSIGN_ERR, 123);
                }
            }
            else if(idsNode->childrenNodes[i]->hashTableItem->datatype == DATATYPE_NUM)
            {
                if(funcCallNode->hashTableItem->funcReturns[i]->datatype == DATATYPE_STRING)
                {
                    errorExit(SEMANTIC_ASSIGN_ERR, 321);
                }
            }
            else if(idsNode->childrenNodes[i]->hashTableItem->datatype == DATATYPE_STRING)
            {
                if(funcCallNode->hashTableItem->funcReturns[i]->datatype != DATATYPE_STRING)
                {
                    errorExit(SEMANTIC_ASSIGN_ERR, 228);
                }
            }

        break;
    default:
        fprintf(stderr,"NOTE processVariableDefStatement - error with assignment\n");
        errorExit(SEMANTIC_ANOTHER_ERR, token->line);
        break;
    }
}

// <statement> --> local id : datatype = <expression_or_func_call>
void processVariableDefStatement(ast_node *varDefNode, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)
{
    // get token, should be ID
    token_t *token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_ID)
    {
        fprintf(stderr, "NOTE: %s - no ID token\n", __func__);
        errorExit(BAD_SYNTAX_ERR, token->line);
    }
    ast_node *variableNode = make_new_node();
    make_new_child(varDefNode, variableNode);

    if(htab_find(hashTableList->first->symtable,token->data.tokenStringVal))
    {
        fprintf(stderr, "NOTE: Has been already declared\n");
        errorExit(SEMANTIC_UNDEF_REDEF_ERR, token->line);
    }

    //TODO check na vsyakij sluchaj
    if((listSearch(hashTableList, token->data.tokenStringVal, FROM_FIRST)) && (listSearch(hashTableList, token->data.tokenStringVal, FROM_FIRST)->type == TYPE_FUNC))
    {
        fprintf(stderr, "%s NOTE: variable can't have the same name as function.\n", __func__);
        errorExit(SEMANTIC_ANOTHER_ERR, token->line);
    }

    variableNode->hashTableItem = htab_lookup_add(hashTableList->first->symtable, token->data.tokenStringVal);

    // get next token, should be :
    token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_COLON)
    {
        fprintf(stderr, "NOTE processVariableDefStatement - no : token\n");
        errorExit(BAD_SYNTAX_ERR, token->line);
    }
    // get next token, should be datatype
    token = getToken(f, dynamicString, tokenStack);
    // TODO changed if condition
    if(!((token->type == TOKEN_INT_KW) || (token->type == TOKEN_NUM_KW) || (token->type == TOKEN_STR_KW)))
    {
        fprintf(stderr, "NOTE: processVariableDefStatement - no datatype token\n");
        errorExit(BAD_SYNTAX_ERR, token->line);
    }

    switch (token->type)    // adding of datatype to variable
        {
            case TOKEN_INT_KW:
                variableNode->hashTableItem->datatype = DATATYPE_INT;
                break;
            case TOKEN_NUM_KW:
                variableNode->hashTableItem->datatype = DATATYPE_NUM;
                break;
            case TOKEN_STR_KW:
                variableNode->hashTableItem->datatype = DATATYPE_STRING;
                break;
            default:
                break;
        }
    
    //get next token, should be =
    token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_ASSIGNMENT)
    {
        ungetToken(token, tokenStack);
        variableNode->nodeType = NODE_NIL;
        variableNode->hashTableItem->type = TYPE_VARIABLE;
        variableNode->hashTableItem->declareFlag = true;    // means that value is NIL!!!!
        variableNode->hashTableItem->defineFlag = false;        // p.s. we're so smart to make new flag
        return;
    }
    // get next token, should be start of expression or start of function call 
    token = getToken(f, dynamicString, tokenStack);
    ast_node *valueNode;
    switch (detectExpressionOrFunctionCall(token->type, f, dynamicString, tokenStack))
    {
        case 1: // expression
            ungetToken(token, tokenStack);

            valueNode = bottomUpAnalysis(hashTableList, f, dynamicString, tokenStack, 0); 
            variableNode->nodeType = valueNode->nodeType;
            // TODO check datatype compatibility
            switch (variableNode->nodeType)
            {
                case NODE_INT_ARG:
                    if(variableNode->hashTableItem->datatype == DATATYPE_STRING)
                    {
                        errorExit(SEMANTIC_ASSIGN_ERR, 122);
                    }
                    if(variableNode->hashTableItem->datatype == DATATYPE_INT)
                    {
                        variableNode->nodeData.intData = valueNode->nodeData.intData;
                        variableNode->hashTableItem->varIntVal = variableNode->nodeData.intData;    
                    }
                    if(variableNode->hashTableItem->datatype == DATATYPE_NUM)
                    {
                        variableNode->nodeData.doubleData = valueNode->nodeData.intData;
                        variableNode->hashTableItem->varNumVal = variableNode->nodeData.doubleData;
                        variableNode->nodeType = NODE_NUM_ARG;
                    }
                    break;
                case NODE_NUM_ARG:
                    if(variableNode->hashTableItem->datatype != DATATYPE_NUM)
                    {
                        errorExit(SEMANTIC_ASSIGN_ERR, 122);
                    }
                    variableNode->nodeData.doubleData = valueNode->nodeData.doubleData;
                    variableNode->hashTableItem->varNumVal = variableNode->nodeData.doubleData;
                    break;
                case NODE_STR_ARG:
                    if(variableNode->hashTableItem->datatype != DATATYPE_STRING)
                    {
                        errorExit(SEMANTIC_ASSIGN_ERR, 122);
                    }

                    variableNode->nodeData.stringData = malloc(sizeof(char) * strlen(valueNode->nodeData.stringData));
                    strcpy(variableNode->nodeData.stringData, valueNode->nodeData.stringData);

                    variableNode->hashTableItem->varStrVal = malloc(sizeof(char) * strlen(variableNode->nodeData.stringData));
                    strcpy(variableNode->hashTableItem->varStrVal, variableNode->nodeData.stringData);
                    break;
                case NODE_TRUE:
                    printf("ASSIGN WITH TRUE\n");
                    errorExit(SEMANTIC_IN_EXPRESSION_TYPES_ERR, 1243);
                case NODE_NIL:
                    variableNode->hashTableItem->type = TYPE_VARIABLE;
                    variableNode->hashTableItem->declareFlag = true;    // means that value is NIL!!!!
                    variableNode->hashTableItem->defineFlag = false;        // p.s. we're so smart to make new flag
                    return;
                default:
                    break;
            }

            break;
        case 2: // function call 
            // TODO UNCHECKED 
            ungetToken(token, tokenStack);
            processFuncCall(varDefNode, hashTableList, f, dynamicString, tokenStack);
            break;
        default:
            fprintf(stderr, "NOTE processVariableDefStatement - error with assignment\n");
            errorExit(BAD_SYNTAX_ERR, token->line);
            break;
    } 
    variableNode->hashTableItem->type = TYPE_VARIABLE;
    variableNode->hashTableItem->declareFlag = true; 
    variableNode->hashTableItem->defineFlag = true; 
}

void processStatement(ast_node *funcDefNode, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)
{
    printf("PROCESS STATEMNER\n");
    // make node for statement 
    ast_node *statementNode = make_new_node();

    token_t *token = getToken(f, dynamicString, tokenStack);
    token_t *nextToken;
    // detect type of statement 
    switch (token->type)
    {
    case TOKEN_LOCAL:
        // set type of the statement node to variable definition statement
        statementNode->nodeType = NODE_VAR_DEF;
        
        // process variable definition
        processVariableDefStatement(statementNode, hashTableList, f, dynamicString, tokenStack); // <statement> --> local id : datatype = <expression_or_func_call>
        break;
    case TOKEN_ID:
        printf("STATEMENT ID CASE\n");
        // get next token to detect type of statement
        nextToken = getToken(f, dynamicString, tokenStack);
        if (nextToken->type == TOKEN_COMMA)
        {
            // return token, should be processed again
            ungetToken(nextToken, tokenStack);
            // return token, should be processed again
            ungetToken(token, tokenStack);
            // set type of the node to multiple assignment node
            statementNode->nodeType = NODE_MULTIPLE_ASSIGN;
            // process multiple assignment statement
            processMultipleAssignment(statementNode, hashTableList, f, dynamicString, tokenStack); // <statement> --> id <more_ids> = <expression_or_func_call>

        }
        else if(nextToken->type == TOKEN_ASSIGNMENT)
        {
            //  return token, should be processed again
            ungetToken(nextToken, tokenStack); 
            //  return token, should be processed again
            ungetToken(token, tokenStack);
            // set type of the node to single assignment node
            statementNode->nodeType = NODE_SINGLE_ASSIGN;
            // process single assignment statement
            processSingleAssignment(statementNode, hashTableList, f, dynamicString, tokenStack); // <statement> --> id = <expression_or_func_call>
        }
        else if(nextToken->type == TOKEN_L_BR)
        {
            //  return token, should be processed again
            ungetToken(nextToken, tokenStack);
            //  return token, should be processed again
            ungetToken(token, tokenStack);
            // set type of the node to function call node
            statementNode->nodeType = NODE_FUNC_CALL;
            // process function call statement 
            processFuncCall(statementNode, hashTableList, f, dynamicString, tokenStack); // <statement> --> id (<list_of_parameters>)
        }
        else
        {
            fprintf(stderr, "NOTE statement - wrong kind of token after ID token\n");
            errorExit(BAD_SYNTAX_ERR, token->line);
        }
        break;
    case TOKEN_IF:
        // set type of the statement node to if statement
        statementNode->nodeType = NODE_IF;
        // process if statement
        processIfStatement(statementNode, hashTableList, f, dynamicString, tokenStack); // <statement> --> if expression then <list_of_statements> else <list_of_statements>  end
        break;
    case TOKEN_WHILE:
        // set type of the statement node to while statement
        statementNode->nodeType = NODE_WHILE;
        // process while statement
        processWhileStatement(statementNode, hashTableList, f, dynamicString, tokenStack); // <statement> --> while expression do <list_of_statements> end
        break;
    case TOKEN_RET:
        // set type of the statement node to return statement
        statementNode->nodeType = NODE_RETURN;
        // process return statement
        processReturnStatement(statementNode, funcDefNode, hashTableList, f, dynamicString, tokenStack); // <statement> --> return <list_of_expressions>
        break;
    default:
        fprintf(stderr, "NOTE statement - wrong kind of token %d\n", token->type);
        errorExit(BAD_SYNTAX_ERR, token->line);
        break;
    }
    // set statement node as a child of function definition node 
    make_new_child(funcDefNode, statementNode);

}

void processDatatypesList(ast_node* funcDeclNode, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack) /// DONE
{
    token_t* token = getToken(f, dynamicString, tokenStack);
    if(!(token->type == TOKEN_INT_KW || token->type == TOKEN_NUM_KW || token->type == TOKEN_STR_KW))
    {
        printf("NOTE: ARGUMENTS IN FUNCTION DECLARATION SHOULD BE JUST DATATYPES\n");
        errorExit(BAD_SYNTAX_ERR, token->line);
    }

    while(token->type == TOKEN_INT_KW || token->type == TOKEN_NUM_KW || token->type == TOKEN_STR_KW)
    {
        htab_data_t* newArgDT = malloc(sizeof(htab_data_t));

        switch (token->type)
        {
            case TOKEN_INT_KW:
                newArgDT->datatype = DATATYPE_INT;
                break;
            case TOKEN_NUM_KW:
                newArgDT->datatype = DATATYPE_NUM;
                break;
            case TOKEN_STR_KW:
                newArgDT->datatype = DATATYPE_STRING;
                break;
            default:
                break;
        }
        makeNewArg(funcDeclNode->hashTableItem, newArgDT);

        token = getToken(f, dynamicString, tokenStack);
        
        if(token->type == TOKEN_COMMA)
        {
            token = getToken(f, dynamicString, tokenStack);
            if(!(token->type == TOKEN_INT_KW || token->type == TOKEN_NUM_KW || token->type == TOKEN_STR_KW))
            {
                errorExit(BAD_SYNTAX_ERR, token->line);
            }
            continue;
        }
        else if (token->type != TOKEN_COMMA && token->type != TOKEN_R_BR)
        {
            errorExit(BAD_SYNTAX_ERR, token->line);
        }
    }
}

void processReturnDatatypesList(ast_node* funcDefNode, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack) /// DONE
{
    token_t* token = getToken(f, dynamicString, tokenStack);
    if(funcDefNode->hashTableItem->declareFlag == 1)
    {
        int countOfRets = 0;
        while(token->type == TOKEN_INT_KW || token->type == TOKEN_NUM_KW || token->type == TOKEN_STR_KW)
        {
            if(countOfRets >= funcDefNode->hashTableItem->countOfReturns)
            {
                printf("BAD AMMOUNT OF RETURNS\n");
                errorExit(SEMANTIC_PARAM_COUNT_ERR,123);
            }
            switch (token->type)
            {
                case TOKEN_INT_KW:
                    if(funcDefNode->hashTableItem->funcReturns[countOfRets]->datatype != DATATYPE_INT)
                    {
                        printf("BAD RETURN DATATYPE\n");
                        errorExit(SEMANTIC_PARAM_COUNT_ERR, 123);
                    }
                    break;
                case TOKEN_NUM_KW:
                    if(funcDefNode->hashTableItem->funcReturns[countOfRets]->datatype != DATATYPE_NUM)
                    {
                        printf("BAD RETURN DATATYPE\n");
                        errorExit(SEMANTIC_PARAM_COUNT_ERR, 123);
                    }
                    break;
                case TOKEN_STR_KW:
                    if(funcDefNode->hashTableItem->funcReturns[countOfRets]->datatype != DATATYPE_STRING)
                    {
                        printf("BAD RETURN DATATYPE\n");
                        errorExit(SEMANTIC_PARAM_COUNT_ERR, 123);
                    }
                    break;
                default:
                    break;
            }
            countOfRets++;
            token = getToken(f, dynamicString, tokenStack);
            
            if(token->type == TOKEN_COMMA)
            {
                token = getToken(f, dynamicString, tokenStack);
                continue;
            }
            else if (token->type == TOKEN_INT_KW || token->type == TOKEN_NUM_KW || token->type == TOKEN_STR_KW)
            {
                errorExit(BAD_SYNTAX_ERR, token->line);
            }
            else 
            {
                ungetToken(token, tokenStack);
                if(countOfRets < funcDefNode->hashTableItem->countOfReturns)
                {
                    printf("BAD AMMOUNT OF RETURNS\n");
                    errorExit(SEMANTIC_PARAM_COUNT_ERR,123);
                }
                return;
            }
        }
    }
    else
    {
        while(token->type == TOKEN_INT_KW || token->type == TOKEN_NUM_KW || token->type == TOKEN_STR_KW)
        {
            htab_data_t* newRet = malloc(sizeof(htab_data_t));

            switch (token->type)
            {
                case TOKEN_INT_KW:
                    newRet->datatype = DATATYPE_INT;
                    break;
                case TOKEN_NUM_KW:
                    newRet->datatype = DATATYPE_NUM;
                    break;
                case TOKEN_STR_KW:
                    newRet->datatype = DATATYPE_STRING;
                    break;
                default:
                    break;
            }
            makeNewRet(funcDefNode->hashTableItem, newRet);

            token = getToken(f, dynamicString, tokenStack);
            
            if(token->type == TOKEN_COMMA)
            {
                token = getToken(f, dynamicString, tokenStack);
                continue;
            }
            else if (token->type == TOKEN_INT_KW || token->type == TOKEN_NUM_KW || token->type == TOKEN_STR_KW)
            {
                errorExit(BAD_SYNTAX_ERR, token->line);
            }
            else 
            {
                ungetToken(token, tokenStack);
                return;
            }
        }
    }

 /*-----------------------------------If function reaches this point - ERROR!-----------------------------------------*/
    fprintf(stderr, "%s ERROR : No datatype in non-process function\n", __func__); 
    errorExit(BAD_SYNTAX_ERR, token->line);
    // TODO counter of lines in program!!!
}

void processParametersList(ast_node* funcDefNode, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack) /////// DONE
{
    token_t *token = getToken(f, dynamicString, tokenStack);
    htab_data_t* newArg;
    htab_data_t* forNoneDeclared;
    htab_key_t tmpKey;
    if(funcDefNode->hashTableItem->declareFlag == 1)
    {   
        int counterArgs = 0;
        while(token->type != TOKEN_R_BR)
        {
            if(counterArgs >= funcDefNode->hashTableItem->countOfArgs)
            {
                printf("BAD AMMOUNT OF PARAMS\n");
                errorExit(SEMANTIC_PARAM_COUNT_ERR,123);
            }
            if(token->type == TOKEN_ID)
            {
                tmpKey = token->data.tokenStringVal;
                token = getToken(f, dynamicString, tokenStack);
                if(token->type == TOKEN_COLON)
                {
                    token = getToken(f, dynamicString, tokenStack);
                    if(token->type == TOKEN_INT_KW || token->type == TOKEN_NUM_KW || token->type == TOKEN_STR_KW)
                    {
                        switch (token->type)
                        {
                            case TOKEN_INT_KW:
                                if(funcDefNode->hashTableItem->funcArgs[counterArgs]->datatype == DATATYPE_INT)
                                {
                                    funcDefNode->hashTableItem->funcArgs[counterArgs]->key = tmpKey;
                                    newArg = htab_lookup_add(hashTableList->first->symtable, tmpKey);
                                    newArg->datatype = DATATYPE_INT;
                                    newArg->declareFlag = 1;
                                    newArg->defineFlag = 1;
                                    newArg->type = TYPE_VARIABLE;
                                }
                                else
                                {
                                    printf("CONFLICTING ARGUMENTS DATATYPES\n");
                                    errorExit(SEMANTIC_PARAM_COUNT_ERR, 123);
                                }
                                break;
                            case TOKEN_NUM_KW:
                                if(funcDefNode->hashTableItem->funcArgs[counterArgs]->datatype == DATATYPE_NUM)
                                {
                                    funcDefNode->hashTableItem->funcArgs[counterArgs]->key = tmpKey;
                                    newArg = htab_lookup_add(hashTableList->first->symtable, tmpKey);
                                    newArg->datatype = DATATYPE_NUM;
                                    newArg->declareFlag = 1;
                                    newArg->defineFlag = 1;
                                    newArg->type = TYPE_VARIABLE;
                                }
                                else
                                {
                                    printf("CONFLICTING ARGUMENTS DATATYPES\n");
                                    errorExit(SEMANTIC_PARAM_COUNT_ERR, 123);
                                }
                                break;
                            case TOKEN_STR_KW:
                                if(funcDefNode->hashTableItem->funcArgs[counterArgs]->datatype == DATATYPE_STRING)
                                {
                                    funcDefNode->hashTableItem->funcArgs[counterArgs]->key = tmpKey;
                                    newArg = htab_lookup_add(hashTableList->first->symtable, tmpKey);
                                    newArg->datatype = DATATYPE_STRING;
                                    newArg->declareFlag = 1;
                                    newArg->defineFlag = 1;
                                    newArg->type = TYPE_VARIABLE;
                                }
                                else
                                {
                                    printf("CONFLICTING ARGUMENTS DATATYPES\n");
                                    errorExit(SEMANTIC_PARAM_COUNT_ERR, 123);
                                }
                                break;
                            default:
                                break;
                        }
                        
                        token = getToken(f, dynamicString, tokenStack);
                        if(token->type == TOKEN_COMMA)
                        {
                            token = getToken(f, dynamicString, tokenStack);
                            if(token->type != TOKEN_ID)
                            {
                                errorExit(BAD_SYNTAX_ERR, token->line);
                            }
                            counterArgs++;
                            continue;
                        }
                        else if (token->type != TOKEN_COMMA && token->type != TOKEN_R_BR)
                        {
                            errorExit(BAD_SYNTAX_ERR, token->line);
                        }
                    }
                    else
                    {
                        fprintf(stderr, "NOTE: expected DATATYPE token\n");
                        errorExit(BAD_SYNTAX_ERR, token->line);    
                    }
                }
                else
                {
                    fprintf(stderr, "NOTE: expected COLON token\n");
                    errorExit(BAD_SYNTAX_ERR, token->line);    
                }
            }
            else
            {
                fprintf(stderr, "NOTE: expected ID token\n");
                errorExit(BAD_SYNTAX_ERR, token->line);
            }
        }
        if(counterArgs < funcDefNode->hashTableItem->countOfArgs-1)
        {
            printf("BAD AMMOUNT OF PARAMS\n");
            errorExit(SEMANTIC_PARAM_COUNT_ERR,123);
        }
    }
    else
    {
        while(token->type != TOKEN_R_BR)
        {
            if(token->type == TOKEN_ID)
            {
                tmpKey = token->data.tokenStringVal;
                token = getToken(f, dynamicString, tokenStack);
                if(token->type == TOKEN_COLON)
                {
                    token = getToken(f, dynamicString, tokenStack);
                    if(token->type == TOKEN_INT_KW || token->type == TOKEN_NUM_KW || token->type == TOKEN_STR_KW)
                    {
                        switch (token->type)
                        {
                            case TOKEN_INT_KW:
                                newArg = createData(tmpKey, DATATYPE_INT);
                                forNoneDeclared = htab_lookup_add(hashTableList->first->symtable, tmpKey);
                                forNoneDeclared->datatype = DATATYPE_INT;
                                forNoneDeclared->declareFlag = 1;
                                forNoneDeclared->defineFlag = 1;
                                forNoneDeclared->type = TYPE_VARIABLE;
                                break;
                            case TOKEN_NUM_KW:
                                newArg = createData(tmpKey, DATATYPE_NUM);
                                forNoneDeclared = htab_lookup_add(hashTableList->first->symtable, tmpKey);
                                forNoneDeclared->datatype = DATATYPE_NUM;
                                forNoneDeclared->declareFlag = 1;
                                forNoneDeclared->defineFlag = 1;
                                forNoneDeclared->type = TYPE_VARIABLE;
                                break;
                            case TOKEN_STR_KW:
                                newArg = createData(tmpKey, DATATYPE_STRING);
                                forNoneDeclared = htab_lookup_add(hashTableList->first->symtable, tmpKey);
                                forNoneDeclared->datatype = DATATYPE_STRING;
                                forNoneDeclared->declareFlag = 1;
                                forNoneDeclared->defineFlag = 1;
                                forNoneDeclared->type = TYPE_VARIABLE;
                                break;
                            default:
                                break;
                        }
                        makeNewArg(funcDefNode->hashTableItem, newArg);
                        
                        token = getToken(f, dynamicString, tokenStack);
                        if(token->type == TOKEN_COMMA)
                        {
                            token = getToken(f, dynamicString, tokenStack);
                            if(token->type != TOKEN_ID)
                            {
                                errorExit(BAD_SYNTAX_ERR, token->line);
                            }
                            continue;
                        }
                        else if (token->type != TOKEN_COMMA && token->type != TOKEN_R_BR)
                        {
                            errorExit(BAD_SYNTAX_ERR, token->line);
                        }
                    }
                    else
                    {
                        fprintf(stderr, "NOTE: expected DATATYPE token\n");
                        errorExit(BAD_SYNTAX_ERR, token->line);    
                    }
                }
                else
                {
                    fprintf(stderr, "NOTE: expected COLON token\n");
                    errorExit(BAD_SYNTAX_ERR, token->line);    
                }
            }
            else
            {
                fprintf(stderr, "NOTE: expected ID token\n");
                errorExit(BAD_SYNTAX_ERR, token->line);
            }
        }
    }
    
}

// <functions> --> id ( )
void processVoidFunctionCall(ast_node *ast, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack) ////// DONE
{
    token_t* token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_ID)
    {
        errorExit(BAD_SYNTAX_ERR, token->line);
    }

    if(!listSearch(hashTableList,token->data.tokenStringVal, FROM_FIRST) ||
        listSearch(hashTableList, token->data.tokenStringVal, FROM_FIRST)->type != TYPE_FUNC)
    {
        errorExit(SEMANTIC_UNDEF_REDEF_ERR, token->line);
    }
    if(listSearch(hashTableList, token->data.tokenStringVal, FROM_FIRST)->countOfReturns != 0)
    {
        printf("YOU ARE TRYING TO CALL NON-VOID FUNCTION\n");
        errorExit(SEMANTIC_ANOTHER_ERR, 123);
    }

    // make node for function declaration
    ast_node *funcVoidFuncCall = make_new_node();
    funcVoidFuncCall->nodeType = NODE_FUNC_VOID_CALL;
    funcVoidFuncCall->nodeData.stringData = token->data.tokenStringVal;

    token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_L_BR)
    {
        printf("ERROR void function call - no ( \n");
        errorExit(BAD_SYNTAX_ERR, token->line);
    }
    token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_R_BR)
    {
        printf("ERROR void function call - no ) \n");
        errorExit(BAD_SYNTAX_ERR, token->line);
    }
    make_new_child(ast, funcVoidFuncCall);
}

// <functions> --> global id : function ( <list_of_datatypes> ) : <list_of_datatypes>
void processFunctionDeclaration(ast_node *ast, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)    //////////// DONE!!!
{
    
    // make node for function declaration
    ast_node *funcDeclNode = make_new_node();
    funcDeclNode->nodeType = NODE_FUNC_DECL;
    // set funcDef node as a child of program root
    make_new_child(ast, funcDeclNode);
    // get next token, should be ID
    token_t *token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_ID)
    {
        // ERROR
        printf("ERROR function declaration - no function ID\n");
        errorExit(BAD_SYNTAX_ERR, token->line);
    }

    funcDeclNode->nodeData.stringData = token->data.tokenStringVal;
    
    if(htab_find(hashTableList->first->symtable, token->data.tokenStringVal))
    {
        //ERROR
        printf("FUNCTION %s was already declared\n", token->data.tokenStringVal);
        errorExit(SEMANTIC_UNDEF_REDEF_ERR, token->line);
    }

    funcDeclNode->hashTableItem = htab_lookup_add(hashTableList->first->symtable, token->data.tokenStringVal);

    // get next token, should be : 
    token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_COLON)
    {
        // ERROR
        printf("ERROR function declaration - no : \n");
        errorExit(BAD_SYNTAX_ERR, token->line);
    }
    // get next token, should be function key word
    token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_FUNC)
    {
        // ERROR
        printf("ERROR function declaration - no function key word \n");
        errorExit(BAD_SYNTAX_ERR, token->line);
    }
    // get next token, should be (
    token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_L_BR)
    {
        // ERROR
        printf("ERROR function declaration - no ( \n");
        errorExit(BAD_SYNTAX_ERR, token->line);
    }

    // get next token, if it's not ), process list of datatypes
    token = getToken(f, dynamicString, tokenStack);

    if(token->type != TOKEN_R_BR)
    {
        ungetToken(token, tokenStack);
        processDatatypesList(funcDeclNode, f, dynamicString, tokenStack);
    }

    // get next token, should be :, else void function
    token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_COLON)
    {
        // // ERROR
        // printf("ERROR function declaration - no : \n");
        ungetToken(token, tokenStack);
        funcDeclNode->hashTableItem->declareFlag = true;
        funcDeclNode->hashTableItem->defineFlag = false;
        return;
    }
    // process list of return datatypes 
    processReturnDatatypesList(funcDeclNode, hashTableList, f, dynamicString, tokenStack);
    funcDeclNode->hashTableItem->declareFlag = true;
    funcDeclNode->hashTableItem->defineFlag = false;
}

// <functions> --> function id ( <list_of_parameters> ) : <list_of_datatypes> <list_of_statements> end
void processFunctionDefinition(ast_node *ast, htab_list_t *hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)
{
    htab_t *levelZeroTable = hashTableList->first->symtable;
    // make node for function definition  
    ast_node *funcDefNode = make_new_node();
    funcDefNode->nodeType = NODE_FUNC_DEF;
    //get next token, should be ID
    token_t *token = getToken(f, dynamicString, tokenStack);
    printf("\n\n\n%s\n\n\n", token->data.tokenStringVal);
    if(token->type != TOKEN_ID)
    {
        // ERROR
        printf("ERROR function definition - no function ID\n");
        errorExit(BAD_SYNTAX_ERR, token->line);
    }

    funcDefNode->nodeData.stringData = token->data.tokenStringVal;

    if(htab_find(hashTableList->first->symtable,token->data.tokenStringVal) && 
       htab_find(hashTableList->first->symtable,token->data.tokenStringVal)->defineFlag)
    {
        //TODO error code
        printf("949");
        errorExit(SEMANTIC_UNDEF_REDEF_ERR, token->line);
    }

    // TODO check compatibility of datatypes of args and returns

    funcDefNode->hashTableItem = htab_lookup_add(levelZeroTable, token->data.tokenStringVal);
    // TODO set type to FUNC TYPE
    funcDefNode->hashTableItem->type = TYPE_FUNC;

    // get next token, should be (
    token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_L_BR)
    {
        // ERROR
        printf("ERROR function definition - no ( for arguments\n");
    }

    token = getToken(f, dynamicString, tokenStack);

    // if token is not ) process function parameters
    int flag = 1;
    if(token->type != TOKEN_R_BR)
    {
        ungetToken(token, tokenStack);
        processParametersList(funcDefNode, hashTableList, f, dynamicString, tokenStack);
        flag = 0;
    }
    if(funcDefNode->hashTableItem->countOfArgs != 0 && flag)
    {
        printf("ARE WE HERE???\n");
        errorExit(SEMANTIC_PARAM_COUNT_ERR,123);
    }

    // get next token, should be :
    token = getToken(f, dynamicString, tokenStack);
    int flag1 = 1;
    if(token->type == TOKEN_COLON) // process list of return datatypes
    {
        processReturnDatatypesList(funcDefNode, hashTableList, f, dynamicString, tokenStack);
        flag1 = 0;
    }
    else ungetToken(token, tokenStack);
    if(funcDefNode->hashTableItem->countOfReturns != 0 && flag1)
    {
        printf("HERE WE ARE???\n");
        errorExit(SEMANTIC_PARAM_COUNT_ERR,123);
    }

    // TODO podumaj

    // get next token, if its not END process list of statements
    token = getToken(f, dynamicString, tokenStack);

    htab_t* levelOneTable = htab_init(43969);               /* To correctly simulate multilevel structure of scope hierarchy we need to */
    htab_list_item_t* newItem = createItem(levelOneTable);      /* make next element in our htab-list */
    insertFirst(hashTableList, newItem);
    while(token->type != TOKEN_END)
    {
        printf("1005: Process statement\n");
        ungetToken(token, tokenStack);
        processStatement(funcDefNode, hashTableList, f, dynamicString, tokenStack);

        token = getToken(f, dynamicString, tokenStack);
    }

    //testScope(hashTableList);

    removeFirst(hashTableList); // whole function has been processed, we can remove first element of scope

    funcDefNode->hashTableItem->declareFlag = true;
    funcDefNode->hashTableItem->defineFlag = true;



    // set funcDef node as a child of program root
    make_new_child(ast, funcDefNode);

     
}

// <prog> --> require "ifj21" <functions> EOF
void processProgramTemplate(ast_node *ast, htab_list_t *hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack) ///// DONE
{
    // check prolog
    token_t *token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_REQ)
    {
        // ERROR
        printf("ERROR not a require token\n");
        errorExit(BAD_SYNTAX_ERR, token->line);
    }
    token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_STR || strcmp(token->data.tokenStringVal, "ifj21") !=0)
    {
        // ERROR
        printf("ERROR not a \"ifj21\" token\n");
        errorExit(BAD_SYNTAX_ERR, token->line);
    }
    // prolog part is OK!
    // check functions block
    token = getToken(f, dynamicString, tokenStack);
    while(token->type != TOKEN_EOF)
    {
        printf("1065: token type %d(%s)\n", token->type, token->data.tokenStringVal);
        if(token->type == TOKEN_FUNC) // <functions> --> function id ( <list_of_parameters> ) : <list_of_datatypes> <list_of_statements> end
        {
            processFunctionDefinition(ast, hashTableList, f, dynamicString, tokenStack);
        }
        else if(token->type == TOKEN_GLOBAL) // <functions> --> global id : function ( <list_of_datatypes> ) : <list_of_datatypes>
        {
            printf("1049: go to function declaration\n");
            processFunctionDeclaration(ast, hashTableList, f, dynamicString, tokenStack);
        }
        else if(token->type == TOKEN_ID) // <functions> --> id ( )
        {
            ungetToken(token, tokenStack);
            processVoidFunctionCall(ast, hashTableList, f, dynamicString, tokenStack);
        }
        else // don't know this rule :(
        {
            //ERROR
            printf("ERROR not a functions block\n");
            errorExit(BAD_SYNTAX_ERR, token->line);
        }
        token = getToken(f, dynamicString, tokenStack);
    }

}

// main function that starts building ast
ast_node *parseAST(htab_t *symTable, FILE *f)
{
    insertBuiltIn(symTable);
    StackTokens tokenStack;
    initStackTokens(&tokenStack);
    DynamicString dynamicString;
    DynamicStringInit(&dynamicString);
    htab_list_t *hashTableList = initList();
    htab_list_item_t *newItem = createItem(symTable);
    insertFirst(hashTableList, newItem);
    ast_node *ast = make_new_node();
    ast->nodeType = NODE_PROG;
    processProgramTemplate(ast, hashTableList, f, &dynamicString, &tokenStack);
    printf("-----------------------------------------------\n");
    testScope(hashTableList);
    printf("-----------------------------------------------\n");
    return ast;
}



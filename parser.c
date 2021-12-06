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
            return 0; // return 0 means that it's error
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
    if(detectExpressionOrFunctionCall(token->type, f, dynamicString, tokenStack) == 1)
    {
        ungetToken(token, tokenStack);
        do
        {
            if(returnCounter > funcDefNode->hashTableItem->countOfReturns)          // return int x, string y, num z
            {
                errorExit(SEMANTIC_PARAM_COUNT_ERR, token->line);                   // must return int, num, string
            }

            ast_node *expressionNode = bottomUpAnalysis(hashTableList, f, dynamicString, tokenStack); // TODO


            if(expressionNode->nodeType == NODE_STR_ARG && funcDefNode->hashTableItem->funcReturns[returnCounter]->datatype != DATATYPE_STRING)
            {
                errorExit(SEMANTIC_PARAM_COUNT_ERR, token->line);
            }
            else if(expressionNode->nodeType == NODE_NUM_ARG &&                               // NUM can't be returned as INT, bit INT CAN be returned as NUM
                (funcDefNode->hashTableItem->funcReturns[returnCounter]->datatype == DATATYPE_INT))
            {
                errorExit(SEMANTIC_PARAM_COUNT_ERR, token->line);
            }
           /* else if(funcDefNode->hashTableItem->funcReturns[returnCounter]->datatype == DATATYPE_NIL && expressionNode->nodeType = NODE_NIL)
            {
                errorExit(SEMANTIC_PARAM_COUNT_ERR, token->line);
            }*/
            


            make_new_child(returnNode, expressionNode);
            token = getToken(f, dynamicString, tokenStack);


            
            if(token->type == TOKEN_COMMA)
            {
                token = getToken(f, dynamicString, tokenStack);
                if(detectExpressionOrFunctionCall(token->type, f, dynamicString, tokenStack) == 1)
                {
                    ungetToken(token, tokenStack);
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
                    errorExit(SEMANTIC_PARAM_COUNT_ERR, token->line);
                }
                return;
            }
            returnCounter++;
        } while (token->type == TOKEN_COMMA);
    }
    
}

// <statement> --> while expression do <list_of_statements> end
void processWhileStatement(ast_node *whileNode, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)
{
    ast_node *whileConditionNode = bottomUpAnalysis(hashTableList, f, dynamicString, tokenStack);
    whileConditionNode->nodeType = NODE_WHILE_CONDITION;
    make_new_child(whileNode, whileConditionNode);
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
    ast_node *ifConditionNode = bottomUpAnalysis(hashTableList, f, dynamicString, tokenStack);
    ifConditionNode->nodeType = NODE_IF_CONDITION;
    make_new_child(ifNode, ifConditionNode);
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
        ungetToken(token, tokenStack);
        processStatement(thenNode, hashTableList, f, dynamicString, tokenStack);
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
        switch (token->type)
        {
        case TOKEN_INT:
            if(funcCallNode->hashTableItem->funcArgs[countActualArgs]->datatype == TYPE_STRING)
            {
                fprintf(stderr, "NOTE: Argument number %d in function %s\n", countActualArgs, token->data.tokenStringVal);
                errorExit(SEMANTIC_PARAM_COUNT_ERR, token->line);
            }
            //TODO check compatibility of INT and NUM
            funcCallNode->hashTableItem->funcArgs[countActualArgs]->varIntVal = token->data.tokenIntVal;
            break;
        case TOKEN_NUM:
        if(funcCallNode->hashTableItem->funcArgs[countActualArgs]->datatype != TYPE_NUM)
            {
                fprintf(stderr, "NOTE: Argument number %d in function %s\n", countActualArgs, token->data.tokenStringVal);
                errorExit(SEMANTIC_PARAM_COUNT_ERR, token->line);
            }
            
            funcCallNode->hashTableItem->funcArgs[countActualArgs]->varNumVal = token->data.tokenNumVal;
            break;
        case TOKEN_STR:
        if(funcCallNode->hashTableItem->funcArgs[countActualArgs]->datatype != TYPE_STRING)
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
    
    printf("\n\n");
    printf("-----------------------");
    testScope(hashTableList);
    printf("-----------------------");
    printf("\n\n");
    
    if(countActualArgs < funcCallNode->hashTableItem->countOfArgs)
        {
            fprintf(stderr, "NOTE: In function %s\n",token->data.tokenStringVal);
            errorExit(SEMANTIC_PARAM_COUNT_ERR, token->line);
        }

    
}

// <statement> --> id = <expression_or_func_call>
void processSingleAssignment(ast_node *singleAssignNode, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)
{
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
    switch (detectExpressionOrFunctionCall(token->type, f, dynamicString, tokenStack))
    {
    case 1: // expression
        // make expressions node that will be parent node for all IDs in assignment
        expressionsNode = make_new_node();
        expressionsNode->nodeType = NODE_VALUES;
        make_new_child(singleAssignNode, expressionsNode);

        ungetToken(token, tokenStack);
        do
        {
            expressionNode = bottomUpAnalysis(hashTableList, f, dynamicString, tokenStack); // TODO
            make_new_child(expressionsNode, expressionNode);
            token = getToken(f, dynamicString, tokenStack);
            if(token->type == TOKEN_COMMA)
            {
                token = getToken(f, dynamicString, tokenStack);
                if(detectExpressionOrFunctionCall(token->type, f, dynamicString, tokenStack) == 1)
                {
                    ungetToken(token, tokenStack);
                }
                else
                {
                    fprintf(stderr, "NOTE processMultipleAssignment - no expression after , token\n");
                    errorExit(BAD_SYNTAX_ERR, token->line);
                }
            }
            else return;
        } while (token->type == TOKEN_COMMA);
        
        break;
    case 2: // function call 
        funcCallNode = make_new_node();
        funcCallNode->nodeType = NODE_FUNC_CALL;
        make_new_child(singleAssignNode, funcCallNode);
        ungetToken(token, tokenStack);
        processFuncCall(funcCallNode, hashTableList, f, dynamicString, tokenStack);
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
        if(token->type != TOKEN_COMMA)
        {
            fprintf(stderr, "ERROR processMultipleAssignment - no , token\n");    
            errorExit(BAD_SYNTAX_ERR, token->line);
        }
        make_new_child(idsNode, idNode);
        token = getToken(f, dynamicString, tokenStack);
    }
    // get next token, should be start of expression or start of function call 
    token = getToken(f, dynamicString, tokenStack);
    ast_node *expressionsNode;
    ast_node *expressionNode;
    ast_node *funcCallNode;
    switch (detectExpressionOrFunctionCall(token->type, f, dynamicString, tokenStack))
    {
    case 1: // expression
        // make expressions node that will be parent node for all IDs in assignment
        expressionsNode = make_new_node();
        expressionsNode->nodeType = NODE_VALUES;
        make_new_child(multAssignNode, expressionsNode);

        ungetToken(token, tokenStack);
        // TODO think about func in expressions list
        do
        {
            expressionNode = bottomUpAnalysis(hashTableList, f, dynamicString, tokenStack); // TODO
            make_new_child(expressionsNode, expressionNode);
            token = getToken(f, dynamicString, tokenStack);
            if(token->type == TOKEN_COMMA)
            {
                token = getToken(f, dynamicString, tokenStack);
                if(detectExpressionOrFunctionCall(token->type, f, dynamicString, tokenStack) == 1)
                {  // if expression 
                    ungetToken(token, tokenStack);
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
                return;
            }
        } while (token->type == TOKEN_COMMA);
        
        break;
    case 2: // function call 
        funcCallNode = make_new_node();
        funcCallNode->nodeType = NODE_FUNC_CALL;
        make_new_child(multAssignNode, funcCallNode);
        ungetToken(token, tokenStack);
        processFuncCall(funcCallNode, hashTableList, f, dynamicString, tokenStack);
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
    variableNode->nodeType = NODE_ID;
    variableNode->nodeData.stringData = token->data.tokenStringVal;

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
        printf("ttoken type: %d\n", token->type);
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
        printf("593\n");
        ungetToken(token, tokenStack);
        variableNode->hashTableItem->type = TYPE_VARIABLE;
        variableNode->hashTableItem->declareFlag = true;    // means that value is NIL!!!!
        variableNode->hashTableItem->defineFlag = false;        // p.s. we're so smart to make new flag
        return;
    }
    // get next token, should be start of expression or start of function call 
    token = getToken(f, dynamicString, tokenStack);
    ast_node *valueNode = make_new_node();
    switch (detectExpressionOrFunctionCall(token->type, f, dynamicString, tokenStack))
    {
        case 1: // expression
            printf("token is: %d\n", token->type);
            ungetToken(token, tokenStack);
            printf("GO to expression\n");
            valueNode = bottomUpAnalysis(hashTableList, f, dynamicString, tokenStack); 
            variableNode->nodeType = valueNode->nodeType;
            // TODO check datatype compatibility
            switch (variableNode->nodeType)
            {
                case NODE_INT_ARG:
                    variableNode->nodeData.intData = valueNode->nodeData.intData;
                    variableNode->hashTableItem->varIntVal = variableNode->nodeData.intData;
                    break;
                case NODE_NUM_ARG:
                    variableNode->nodeData.doubleData = valueNode->nodeData.doubleData;
                    variableNode->hashTableItem->varNumVal = variableNode->nodeData.doubleData;
                    break;
                case NODE_STR_ARG:
                    variableNode->nodeData.stringData = valueNode->nodeData.stringData;
                    variableNode->hashTableItem->varStrVal = malloc(sizeof(char) * strlen(variableNode->nodeData.stringData));
                    strcpy(variableNode->hashTableItem->varStrVal, variableNode->nodeData.stringData);
                    break;
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
            errorExit(SEMANTIC_ANOTHER_ERR, token->line);
            break;
    } 
    variableNode->hashTableItem->type = TYPE_VARIABLE;
    variableNode->hashTableItem->declareFlag = true; 
    variableNode->hashTableItem->defineFlag = true; 
}

void processStatement(ast_node *funcDefNode, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)
{
    // make node for statement 
    ast_node *statementNode = make_new_node();

    token_t *token = getToken(f, dynamicString, tokenStack);
    token_t *nextToken;

    printf("token type: %d\n", token->type);
    // detect type of statement 
    switch (token->type)
    {
    case TOKEN_LOCAL:
        // set type of the statement node to variable definition statement
        statementNode->nodeType = NODE_VAR_DEF;
        printf("632\n");
        // process variable definition
        processVariableDefStatement(statementNode, hashTableList, f, dynamicString, tokenStack); // <statement> --> local id : datatype = <expression_or_func_call>
        break;
    case TOKEN_ID:
        // get next token to detect type of statement
        nextToken = getToken(f, dynamicString, tokenStack);
        printf("Здуся3\n%s\n\n",funcDefNode->nodeData.stringData);
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
             printf("Здуся4\n%s\n\n",funcDefNode->nodeData.stringData);
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
        fprintf(stderr, "NOTE statement - wrong kind of token\n");
        errorExit(BAD_SYNTAX_ERR, token->line);
        break;
    }
    // set statement node as a child of function definition node 
    make_new_child(funcDefNode, statementNode);
}

void processDatatypesList(ast_node* funcDeclNode, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)
{
    token_t* token = getToken(f, dynamicString, tokenStack);
    if(!(token->type == TOKEN_INT_KW || token->type == TOKEN_NUM_KW || token->type == TOKEN_STR_KW))
            {
                errorExit(BAD_SYNTAX_ERR, token->line);
            }
    while(token->type == TOKEN_INT_KW || token->type == TOKEN_NUM_KW || token->type == TOKEN_STR_KW)
    {
        printf("712\n");
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

void processReturnDatatypesList(ast_node* funcDefNode, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)
{
    token_t* token = getToken(f, dynamicString, tokenStack);

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

 /*-----------------------------------If function reaches this point - ERROR!-----------------------------------------*/
    fprintf(stderr, "%s ERROR : No datatype in non-process function\n", __func__); 
    errorExit(BAD_SYNTAX_ERR, token->line);
    // TODO counter of lines in program!!!
}

void processParametersList(ast_node* funcDefNode, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)
{
    token_t *token = getToken(f, dynamicString, tokenStack);
    htab_data_t* newArg;
    htab_key_t tmpKey;
    
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
                            break;
                        case TOKEN_NUM_KW:
                            newArg = createData(tmpKey, DATATYPE_NUM);
                            break;
                        case TOKEN_STR_KW:
                            newArg = createData(tmpKey, DATATYPE_STRING);
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

// <functions> --> id ( )
void processVoidFunctionCall(ast_node *ast, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)
{
    token_t* token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_ID)
    {
        printf("828\n");
        errorExit(BAD_SYNTAX_ERR, token->line);
    }

    if(!listSearch(hashTableList,token->data.tokenStringVal, FROM_FIRST) ||
        listSearch(hashTableList, token->data.tokenStringVal, FROM_FIRST)->type != TYPE_FUNC)
    {

        printf("834, %d\n", listSearch(hashTableList, token->data.tokenStringVal, FROM_FIRST)->type);
        errorExit(SEMANTIC_UNDEF_REDEF_ERR, token->line);
    }

    // make node for function declaration
    ast_node *funcVoidFuncCall = make_new_node();
    funcVoidFuncCall->nodeType = NODE_FUNC_VOID_CALL;
    funcVoidFuncCall->nodeData.stringData = token->data.tokenStringVal;

    token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_L_BR)
    {
        // ERROR
        printf("ERROR void function call - no ( \n");
        errorExit(BAD_SYNTAX_ERR, token->line);
    }
    token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_R_BR)
    {
        // ERROR
        printf("ERROR void function call - no ) \n");
        errorExit(BAD_SYNTAX_ERR, token->line);
    }
    make_new_child(ast, funcVoidFuncCall);
}

// <functions> --> global id : function ( <list_of_datatypes> ) : <list_of_datatypes>
void processFunctionDeclaration(ast_node *ast, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)
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
    printf("882\n");
    if(htab_find(hashTableList->first->symtable, token->data.tokenStringVal))
    {
        //ERROR
        printf("876\n");
        errorExit(SEMANTIC_UNDEF_REDEF_ERR, token->line);
    }
    printf("889\n");
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
        processDatatypesList(funcDeclNode, hashTableList, f, dynamicString, tokenStack);  // TODO think about function ungetToken(), then there will be one function for datatypes list and return datatypes list
    }
    printf("921\n");
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
    printf("935\n");
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
    if(token->type != TOKEN_R_BR)
    {
        ungetToken(token, tokenStack);
        processParametersList(funcDefNode, hashTableList, f, dynamicString, tokenStack);
    }

    printf("Здуся\n%s\n\n",funcDefNode->nodeData.stringData);
    // get next token, should be :
    token = getToken(f, dynamicString, tokenStack);
    if(token->type == TOKEN_COLON) // process list of return datatypes
    {
        processReturnDatatypesList(funcDefNode, hashTableList, f, dynamicString, tokenStack);
        
    }
    else ungetToken(token, tokenStack);

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

    removeFirst(hashTableList); // whole function has been processed, we can remove first element of scope

    funcDefNode->hashTableItem->declareFlag = true;
    funcDefNode->hashTableItem->defineFlag = true;

    // set funcDef node as a child of program root
    make_new_child(ast, funcDefNode);


}

// <prog> --> require "ifj21" <functions> EOF
void processProgramTemplate(ast_node *ast, htab_list_t *hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)
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
    //insertBuiltIn(symTable);
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
    return ast;
}



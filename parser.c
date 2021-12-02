#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

#include "scanner.h"

void processStatement(ast_node *funcDefNode, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack);

FILE* openFile(int argc, char** argv)
{
    FILE* f = (argc > 1) ? fopen(argv[1], "r") : stdin;
    return f;
}

int detectExpressionOrFunctionCall(tokenType_t tokenType, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)
{
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

ast_node *processExpression()
{
    ast_node *expressionNode = make_new_node();
    return expressionNode;
}

// <statement> --> return <list_of_expressions>
void processReturnStatement(ast_node *returnNode, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)
{
    token_t *token = getToken(f, dynamicString, tokenStack);
    if(detectExpressionOrFunctionCall(token->type, f, dynamicString, tokenStack) == 1)
    {
        ungetToken(token, tokenStack);
        do
        {
            ast_node *expressionNode = processExpression(); // TODO
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
                    // ERROR
                    printf("ERROR processMultipleAssignment - no expression after , token\n");
                }
            }
            else
            {
                ungetToken(token, tokenStack);
                return;
            }
        } while (token->type == TOKEN_COMMA);
    }
    
}

// <statement> --> while expression do <list_of_statements> end
void processWhileStatement(ast_node *whileNode, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)
{
    ast_node *whileConditionNode = processExpression();
    whileConditionNode->nodeType = NODE_WHILE_CONDITION;
    make_new_child(whileNode, whileConditionNode);
    token_t *token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_DO)
    {
        // ERROR
        printf("ERROR processWhileStatement - no do token\n");
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
    ast_node *ifConditionNode = processExpression();
    ifConditionNode->nodeType = NODE_IF_CONDITION;
    make_new_child(ifNode, ifConditionNode);
    token_t *token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_THEN)
    {
        // ERROR
        printf("ERROR processIfStatement - no then token\n");
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
        // ERROR
        printf("ERROR processFuncCall - no ID token\n");
    }
    if(!listSearch(hashTableList, token->data.tokenStringVal, FROM_FIRST))
    {
        // ERROR function with this name wasn't found
        exit(228);
    }
    else if(listSearch(hashTableList, token->data.tokenStringVal, FROM_FIRST)->type != TYPE_FUNC)
    {
        // ERROR you want to call something, what isn't function
        exit(228);
    }

    funcCallNode->nodeData.stringData = token->data.tokenStringVal;
    copyDataFuncCall(listSearch(hashTableList, token->data.tokenStringVal, FROM_FIRST), funcCallNode->hashTableItem);


    // get token, should be (
    token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_L_BR)
    {
        // ERROR
        printf("ERROR processFuncCall - no ( token\n");
    }

    token = getToken(f, dynamicString, tokenStack);
    int countActualArgs = 0; 
    // get token, if it's not ) token, process arguments
    while (token->type != TOKEN_R_BR)
    {
        if(countActualArgs > funcCallNode->hashTableItem->countOfArgs)
        {
            fprintf(stderr, "%s ERROR: You're trying to pass to lot arguments to function %s ! Exit...", __func__, token->data.tokenStringVal);
            exit(228);
        }
        switch (token->type)
        {
        case TOKEN_INT:
            if(funcCallNode->hashTableItem->funcArgs[countActualArgs]->datatype == TYPE_STRING)
            {
                fprintf(stderr, "%s ERROR: To argument number %d you're trying to pass variable with incompatible datatype in function %s", __func__, countActualArgs, token->data.tokenStringVal);
                exit(228);
            }
            //TODO check compatibility of INT and NUM
            funcCallNode->hashTableItem->funcArgs[countActualArgs]->varIntVal = token->data.tokenIntVal;
            break;
        case TOKEN_NUM:
        if(funcCallNode->hashTableItem->funcArgs[countActualArgs]->datatype != TYPE_NUM)
            {
                fprintf(stderr, "%s ERROR: To argument number %d you're trying to pass variable with incompatible datatype in function %s", __func__, countActualArgs, token->data.tokenStringVal);
                exit(228);
            }
            
            funcCallNode->hashTableItem->funcArgs[countActualArgs]->varNumVal = token->data.tokenNumVal;
            break;
        case TOKEN_STR:
        if(funcCallNode->hashTableItem->funcArgs[countActualArgs]->datatype != TYPE_STRING)
            {
                fprintf(stderr, "%s ERROR: To argument number %d you're trying to pass variable with incompatible datatype in function %s", __func__, countActualArgs, token->data.tokenStringVal);
                exit(228);
            }

            funcCallNode->hashTableItem->funcArgs[countActualArgs]->varStrVal = token->data.tokenStringVal;
            break;
        case TOKEN_ID:
            switch (token->type)
            {
            case TYPE_INT:
                if(funcCallNode->hashTableItem->funcArgs[countActualArgs]->datatype == TYPE_STRING)
                {
                    fprintf(stderr, "%s ERROR: To argument number %d you're trying to pass variable with incompatible datatype in function %s", __func__, countActualArgs, token->data.tokenStringVal);
                    exit(228);
                }
                // TODO int-num compatibility
                funcCallNode->hashTableItem->funcArgs[countActualArgs]->varIntVal = token->data.tokenIntVal;
                break;
            case TYPE_NUM:
                if(funcCallNode->hashTableItem->funcArgs[countActualArgs]->datatype != TYPE_NUM)
                {
                    fprintf(stderr, "%s ERROR: To argument number %d you're trying to pass variable with incompatible datatype in function %s", __func__, countActualArgs, token->data.tokenStringVal);
                    exit(228);
                }

                funcCallNode->hashTableItem->funcArgs[countActualArgs]->varNumVal = token->data.tokenNumVal;
                break;
            case TYPE_STRING:
                if(funcCallNode->hashTableItem->funcArgs[countActualArgs]->datatype != TYPE_STRING)
                {
                    fprintf(stderr, "%s ERROR: To argument number %d you're trying to pass variable with incompatible datatype in function %s", __func__, countActualArgs, token->data.tokenStringVal);
                    exit(228);
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
            // ERROR
            printf("ERROR processFuncCall - wrong argument token\n");
            break;
        }
        
        token = getToken(f, dynamicString, tokenStack);
        countActualArgs++;
    }
    

}

// <statement> --> id = <expression_or_func_call>
void processSingleAssignment(ast_node *singleAssignNode, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)
{
    token_t *token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_ID)
    {
        // ERROR
        printf("ERROR processSingleAssignment - no ID token\n");
        exit(228);
    }
    ast_node *idNode = make_new_node();
    idNode->nodeType = NODE_ID;
    idNode->nodeData.stringData = token->data.tokenStringVal;

    if(!listSearch(hashTableList, token->data.tokenStringVal, FROM_FIRST))
    {
        //TODO error code (isn't declared)
        exit(228);
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
        // ERROR
        printf("ERROR processSingleAssignment - no = token\n");
        exit(228);
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

        ungetToken(token, tokenStack); // TODO
        do
        {
            expressionNode = processExpression(); // TODO
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
                    // ERROR
                    printf("ERROR processMultipleAssignment - no expression after , token\n");
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
        // ERROR
        printf("ERROR processVariableDefStatement - error with assignment\n");
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
            // ERROR
            printf("ERROR processMultipleAssignment - no ID token\n");   
            exit(228); 
        }


        ast_node *idNode = make_new_node();
        idNode->nodeType = NODE_ID;
        idNode->nodeData.stringData = token->data.tokenStringVal;
        if(!listSearch(hashTableList, token->data.tokenStringVal, FROM_FIRST))
        {
            //TODO error code (isn't declared)
            exit(228);
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
            // ERROR
            printf("ERROR processMultipleAssignment - no , token\n");    
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
            expressionNode = processExpression(); // TODO
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
                    // ERROR
                    printf("ERROR processMultipleAssignment - no expression after , token\n");
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
        ungetToken(token, tokenStack); // TODO
        processFuncCall(funcCallNode, hashTableList, f, dynamicString, tokenStack);
        break;
    default:
        // ERROR
        printf("ERROR processVariableDefStatement - error with assignment\n");
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
        // ERROR
        printf("ERROR processVariableDefStatement - no ID token\n");
    }
    ast_node *variableNode = make_new_node();
    make_new_child(varDefNode, variableNode);
    variableNode->nodeType = NODE_ID;
    variableNode->nodeData.stringData = token->data.tokenStringVal;

    if(htab_find(hashTableList->first->symtable,token->data.tokenStringVal))
    {
        //TODO error code
        exit(228); // variable with this name has been already
    }

    //TODO check na vsyakij sluchaj
    if((listSearch(hashTableList, token->data.tokenStringVal, FROM_FIRST)) && (listSearch(hashTableList, token->data.tokenStringVal, FROM_FIRST)->type == TYPE_FUNC))
    {
        fprintf(stderr, "%s ERROR: variable can't have the same name as function. Exit...\n", __func__);
        exit(228);
    }

    variableNode->hashTableItem = htab_lookup_add(hashTableList->first->symtable, token->data.tokenStringVal);

    // get next token, should be :
    token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_COLON)
    {
        // ERROR
        printf("ERROR processVariableDefStatement - no : token\n");
    }
    // get next token, should be datatype
    token = getToken(f, dynamicString, tokenStack);
    if((token->type != TOKEN_INT_KW) || (token->type != TOKEN_NUM_KW) || (token->type != TOKEN_STR_KW))
    {
        // ERROR
        printf("ERROR processVariableDefStatement - no datatype token\n");
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
        variableNode->hashTableItem->declareFlag = true;    // means that value is NIL!!!!
        variableNode->hashTableItem->defineFlag = false;        // p.s. we're so smart to make new flag
        return;
    }
    // get next token, should be start of expression or start of function call 
    token = getToken(f, dynamicString, tokenStack);
    switch (detectExpressionOrFunctionCall(token->type, f, dynamicString, tokenStack))
    {
    case 1: // expression
        ungetToken(token, tokenStack);
        processExpression(); // TODO
        break;
    case 2: // function call 
        ungetToken(token, tokenStack);
        processFuncCall(varDefNode, hashTableList, f, dynamicString, tokenStack);
        break;
    default:
        // ERROR
        printf("ERROR processVariableDefStatement - error with assignment\n");
        break;
    } 
}

void processStatement(ast_node *funcDefNode, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)
{
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
            // ERROR
            printf("ERROR statement - wrong kind of token after ID token\n");
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
        processReturnStatement(statementNode, hashTableList, f, dynamicString, tokenStack); // <statement> --> return <list_of_expressions>
        break;
    default:
        // ERROR
        printf("ERROR statement - wrong kinf of token\n");
        break;
    }
    // set statement node as a child of function definition node 
    make_new_child(funcDefNode, statementNode);
}

void processDatatypesList(ast_node* funcDeclNode, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)
{
    token_t* token = getToken(f, dynamicString, tokenStack);
    
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
            continue;
        }
        else return;
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
        else return;
    }

 /*-----------------------------------If function reaches this point - ERROR!-----------------------------------------*/
    fprintf(stderr, "%s ERROR : No datatype in non-process function\n", __func__); 
    exit(228);
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
                        continue;
                    }
                }
                // TODO ERRORS
            }
        }
    }
}

// <functions> --> id ( )
void processVoidFunctionCall(ast_node *ast, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)
{
    token_t* token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_ID)
    {
        exit(228);
    }

    if(!listSearch(hashTableList,token->data.tokenStringVal, FROM_SECOND) ||
        listSearch(hashTableList, token->data.tokenStringVal, FROM_SECOND)->type != TYPE_FUNC)
    {
        exit(228);
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
    }
    token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_R_BR)
    {
        // ERROR
        printf("ERROR void function call - no ) \n");
    }
    make_new_child(ast, funcVoidFuncCall);
}

// <functions> --> global id : function ( <list_of_datatypes> ) : <list_of_datatypes>
void processFunctionDeclaration(ast_node *ast, htab_list_t* hashTableList, FILE *f, DynamicString *dynamicString, StackTokens *tokenStack)
{
    //TODO check if function was declared and then DEFINED with same datatypes of args and returns

    // make node for function declaration
    ast_node *funcDeclNode = make_new_node();
    funcDeclNode->nodeType = NODE_FUNC_DECL;
    // get next token, should be ID
    token_t *token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_ID)
    {
        // ERROR
        printf("ERROR function declaration - no function ID\n");
    }
    funcDeclNode->nodeData.stringData = token->data.tokenStringVal;
    
    if(htab_find(hashTableList->first->symtable, token->data.tokenStringVal))
    {
        //ERROR
        exit(228);
    }

    //funcDefNode->hashTableItem = htab_lookup_add(levelZeroTable, token->data.tokenStringVal);

    // get next token, should be : 
    token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_COLON)
    {
        // ERROR
        printf("ERROR function declaration - no : \n");
        exit(228);
    }
    // get next token, should be function key word
    token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_FUNC)
    {
        // ERROR
        printf("ERROR function declaration - no function key word \n");
    }
    // get next token, should be (
    token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_L_BR)
    {
        // ERROR
        printf("ERROR function declaration - no ( \n");
    }
    // get next token, if it's not ), process list of datatypes
    token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_R_BR)
    {
        ungetToken(token, tokenStack);
        processDatatypesList(funcDeclNode, hashTableList, f, dynamicString, tokenStack);  // TODO think about function ungetToken(), then there will be one function for datatypes list and return datatypes list
    }
    // get next token, should be :
    token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_COLON)
    {
        // ERROR
        printf("ERROR function declaration - no : \n");
    }
    // process list of return datatypes 
    processReturnDatatypesList(funcDeclNode, hashTableList, f, dynamicString, tokenStack);

    funcDeclNode->hashTableItem->declareFlag = true;
    funcDeclNode->hashTableItem->defineFlag = false;

    // set funcDef node as a child of program root
    make_new_child(ast, funcDeclNode);

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
    if(token->type != TOKEN_ID)
    {
        // ERROR
        printf("ERROR function definition - no function ID\n");
    }

    funcDefNode->nodeData.stringData = token->data.tokenStringVal;

    if(htab_find(hashTableList->first->symtable,token->data.tokenStringVal) && 
       htab_find(hashTableList->first->symtable,token->data.tokenStringVal)->defineFlag)
    {
        //TODO error code
        exit(228);
    }

    // TODO check compatibility of datatypes of args and returns

    funcDefNode->hashTableItem = htab_lookup_add(levelZeroTable, token->data.tokenStringVal);


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
    }
    token = getToken(f, dynamicString, tokenStack);
    if(token->type != TOKEN_STR || strcmp(token->data.tokenStringVal, "ifj21") !=0)
    {
        // ERROR
        printf("ERROR not a \"ifj21\" token\n");
    }
    // prolog part is OK!
    // check functions block
    token = getToken(f, dynamicString, tokenStack);
    while(token->type != TOKEN_EOF)
    {
        if(token->type == TOKEN_FUNC) // <functions> --> function id ( <list_of_parameters> ) : <list_of_datatypes> <list_of_statements> end
        {
            processFunctionDefinition(ast, hashTableList, f, dynamicString, tokenStack);
        }
        else if(token->type == TOKEN_GLOBAL) // <functions> --> global id : function ( <list_of_datatypes> ) : <list_of_datatypes>
        {
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
        }
        token = getToken(f, dynamicString, tokenStack);
    }

}

// main function that starts building ast
ast_node *parseAST(htab_t *symTable, FILE *f)
{
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



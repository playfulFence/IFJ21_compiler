#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "tree.h"
#include "htab.h"
#include "scanner.h"

FILE* openFile(int argc, char** argv)
{
    FILE* f = (argc > 1) ? fopen(argv[1], "r") : stdin;
    return f;
}

int detectExpressionOrFunctionCall(tokenType_t tokenType, FILE *f)
{
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
        // TODO can be replaced by processing type of id(function name or variable name) by using hashtable
        token_t nextToken = getToken(f);
        if(nextToken.type == TOKEN_L_BR)
        {
            ungetToken(); // TODO
            return 2; // return 2 means that it's function call
        }
        else if(nextToken.type == TOKEN_PLUS || nextToken.type == TOKEN_MINUS || 
                nextToken.type == TOKEN_MULT || nextToken.type == TOKEN_S_BS ||
                nextToken.type == TOKEN_D_BS || nextToken.type == TOKEN_CONC)
        {
            ungetToken(); //TODO
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

void processExpression()
{

}

// <statement> --> return <list_of_expressions>
void processReturnStatement(ast_node *returnNode, htab_t **symTable, FILE *f)
{
    token_t token = getToken(f);
    if(detectExpressionOrFunctionCall(token.type, f) == 1)
    {
        ungetToken();
        do
        {
            ast_node *expressionNode = processExpression(); // TODO
            make_new_child(returnNode, expressionNode);
            token = getToken(f);
            if(token.type == TOKEN_COMMA)
            {
                token = getToken(f);
                if(detectExpressionOrFunctionCall(token.type, f) == 1)
                {
                    ungetToken();
                }
                else
                {
                    // ERROR
                    printf("ERROR processMultipleAssignment - no expression after , token\n");
                }
            }
            else
            {
                //TODO end of assignment 
            }
        } while (token.type == TOKEN_COMMA);
    }
    
}

// <statement> --> while expression do <list_of_statements> end
void processWhileStatement(ast_node *whileNode, htab_t **symTable, FILE *f)
{
    ast_node *whileConditionNode = processExpression();
    whileConditionNode->nodeType = NODE_WHILE_CONDITION;
    make_new_child(whileNode, whileConditionNode);
    token_t token = getToken(f);
    if(token.type != TOKEN_DO)
    {
        // ERROR
        printf("ERROR processWhileStatement - no do token\n");
    }
    ast_node *whileDoNode = make_new_node();
    whileDoNode->nodeType = NODE_WHILE_DO;
    make_new_child(whileNode, whileDoNode);
    // get next token, if its not END process list of statements
    token = getToken(f);
    while(token.type != TOKEN_END)
    {
        ungetToken();//TODO
        processStatement(whileDoNode, symTable, f);
    }

}

// <statement> --> if expression then <list_of_statements> else <list_of_statements>  end
void processIfStatement(ast_node *ifNode, htab_t **symTable, FILE *f)
{
    ast_node *ifConditionNode = processExpression();
    ifConditionNode->nodeType = NODE_IF_CONDITION;
    make_new_child(ifNode, ifConditionNode);
    token_t token = getToken(f);
    if(token.type != TOKEN_THEN)
    {
        // ERROR
        printf("ERROR processIfStatement - no then token\n");
    }
    ast_node *thenNode = make_new_node();
    thenNode->nodeType = NODE_IF_THEN;
    make_new_child(ifNode, thenNode);
    // get next token, if its not END process list of statements
    token = getToken(f);
    while(token.type != TOKEN_ELSE)
    {
        ungetToken();//TODO
        processStatement(thenNode, symTable, f);
    }
    ast_node *elseNode = make_new_node();
    elseNode->nodeType = NODE_IF_ELSE;
    make_new_child(ifNode, elseNode);
    token = getToken(f);
    while(token.type != TOKEN_END)
    {
        ungetToken();//TODO
        processStatement(elseNode, symTable, f);
    }
}

// <statement> --> id (<list_of_arguments>)
void processFuncCall(ast_node *funcCallNode, htab_t **symTable, FILE *f)
{   
    // get token, should be ID
    token_t token = getToken(f);
    if(token.type != TOKEN_ID)
    {
        // ERROR
        printf("ERROR processFuncCall - no ID token\n");
    }
    funcCallNode->nodeData.stringData = token.data.tokenStringVal;
    // get token, should be (
    token = getToken(f);
    if(token.type != TOKEN_L_BR)
    {
        // ERROR
        printf("ERROR processFuncCall - no ( token\n");
    }
    // get token, if it's not ) token, process arguments
    while (token.type != TOKEN_R_BR)
    {
        ast_node *parameterNode = make_new_node();
        switch (token.type)
        {
        case TOKEN_INT:
            parameterNode->nodeType = NODE_INT_ARG;
            parameterNode->nodeData.intData = token.data.tokenIntVal;
            break;
        case TOKEN_NUM:
            parameterNode->nodeType = NODE_NUM_ARG;
            parameterNode->nodeData.doubleData = token.data.tokenNumVal;
            break;
        case TOKEN_STR:
            parameterNode->nodeType = NODE_STR_ARG;
            parameterNode->nodeData.stringData = token.data.tokenStringVal;
            break;
        case TOKEN_ID:
            parameterNode->nodeType = NODE_ID_ARG;
            parameterNode->nodeData.stringData = token.data.tokenStringVal;
            break;
        case TOKEN_COMMA:
            token = getToken(f);
            continue;
        default:
            // ERROR
            printf("ERROR processFuncCall - wrong argument token\n");
            break;
        }
        make_new_child(funcCallNode, parameterNode);
        token = getToken(f);
    }
    

}

// <statement> --> id = <expression_or_func_call>
void processSingleAssignment(ast_node *singleAssignNode, htab_t **symTable, FILE *f)
{
    token_t token = getToken(f);
    if(token.type != TOKEN_ID)
    {
        // ERROR
        printf("ERROR processSingleAssignment - no ID token\n");
    }
    ast_node *idNode = make_new_node();
    idNode->nodeType = NODE_ID;
    idNode->nodeData.stringData = token.data.tokenStringVal;
    make_new_child(singleAssignNode, idNode);
    token = getToken(f);
    if(token.type != TOKEN_ASSIGNMENT)
    {
        // ERROR
        printf("ERROR processSingleAssignment - no = token\n");
    }
    // get next token, should be start of expression or start of function call 
    token = getToken(f);
    switch (detectExpressionOrFunctionCall(token.type, f))
    {
    case 1: // expression
        // make expressions node that will be parent node for all IDs in assignment
        ast_node *expressionsNode = make_new_node();
        expressionsNode->nodeType = NODE_VALUES;
        make_new_child(singleAssignNode, expressionsNode);

        ungetToken(); // TODO
        do
        {
            ast_node *expressionNode = processExpression(); // TODO
            make_new_child(expressionsNode, expressionNode);
            token = getToken(f);
            if(token.type == TOKEN_COMMA)
            {
                token = getToken(f);
                if(detectExpressionOrFunctionCall(token.type, f) == 1)
                {
                    ungetToken();
                }
                else
                {
                    // ERROR
                    printf("ERROR processMultipleAssignment - no expression after , token\n");
                }
            }
        } while (token.type == TOKEN_COMMA);
        
        break;
    case 2: // function call 
        ast_node *funcCallNode = make_new_node();
        funcCallNode->nodeType = NODE_FUNC_CALL;
        make_new_child(singleAssignNode, funcCallNode);
        ungetToken(); // TODO
        processFuncCall(funcCallNode, symTable, f);
        break;
    default:
        // ERROR
        printf("ERROR processVariableDefStatement - error with assignment\n");
        break;
    }

}

// <statement> --> id <more_ids> = <expression_or_func_call>
void processMultipleAssignment(ast_node *multAssignNode, htab_t **symTable, FILE *f)
{   
    // make IDs node that will be parent node for all IDs in assignment
    ast_node *idsNode = make_new_node();
    idsNode->nodeType = NODE_IDS;
    make_new_child(multAssignNode, idsNode);
    // process IDs 
    token_t token = getToken(f);
    while(token.type != TOKEN_ASSIGNMENT)
    {
        if(token.type != TOKEN_ID)
        {
            // ERROR
            printf("ERROR processMultipleAssignment - no ID token\n");    
        }
        ast_node *idNode = make_new_node();
        idNode->nodeType = NODE_ID;
        idNode->nodeData.stringData = token.data.tokenStringVal;
        token = getToken(f);
        if(token.type != TOKEN_COMMA)
        {
            // ERROR
            printf("ERROR processMultipleAssignment - no , token\n");    
        }
        make_new_child(idsNode, idNode);
        token = getToken(f);
    }
    // get next token, should be start of expression or start of function call 
    token = getToken(f);
    switch (detectExpressionOrFunctionCall(token.type, f))
    {
    case 1: // expression
        // make expressions node that will be parent node for all IDs in assignment
        ast_node *expressionsNode = make_new_node();
        expressionsNode->nodeType = NODE_VALUES;
        make_new_child(multAssignNode, expressionsNode);

        ungetToken(); // TODO
        do
        {
            ast_node *expressionNode = processExpression(); // TODO
            make_new_child(expressionsNode, expressionNode);
            token = getToken(f);
            if(token.type == TOKEN_COMMA)
            {
                token = getToken(f);
                if(detectExpressionOrFunctionCall(token.type, f) == 1)
                {
                    ungetToken();
                }
                else
                {
                    // ERROR
                    printf("ERROR processMultipleAssignment - no expression after , token\n");
                }
            }
            else
            {
                //TODO end of assignment 
            }
        } while (token.type == TOKEN_COMMA);
        
        break;
    case 2: // function call 
        ast_node *funcCallNode = make_new_node();
        funcCallNode->nodeType = NODE_FUNC_CALL;
        make_new_child(multAssignNode, funcCallNode);
        ungetToken(); // TODO
        processFuncCall(funcCallNode, symTable, f);
        break;
    default:
        // ERROR
        printf("ERROR processVariableDefStatement - error with assignment\n");
        break;
    }
}

// <statement> --> local id : datatype = <expression_or_func_call>
void processVariableDefStatement(ast_node *varDefNode, htab_t **symTable, FILE *f)
{
    // get token, should be ID
    token_t token = getToken(f);
    if(token.type != TOKEN_ID)
    {
        // ERROR
        printf("ERROR processVariableDefStatement - no ID token\n");
    }
    ast_node *variableNode = make_new_node();
    variableNode->nodeType = NODE_ID;
    variableNode->nodeData.stringData = token.data.tokenStringVal;
    // get next token, should be :
    token = getToken(f);
    if(token.type != TOKEN_COLON)
    {
        // ERROR
        printf("ERROR processVariableDefStatement - no : token\n");
    }
    // get next token, should be datatype
    token = getToken(f);
    if(token.type != TOKEN_INT_KW || token.type != TOKEN_NUM_KW || token.type != TOKEN_STR_KW)
    {
        // ERROR
        printf("ERROR processVariableDefStatement - no datatype token\n");
    }
    // TODO process variable and datatype 
    //
    //get next token, should be =
    token = getToken(f);
    if(token.type != TOKEN_ASSIGNMENT)
    {
        // TODO unget token, process variable, set value to nil and return from function, it's the end of variable definition statement
    }
    // get next token, should be start of expression or start of function call 
    token = getToken(f);
    switch (detectExpressionOrFunctionCall(token.type, f))
    {
    case 1: // expression
        ungetToken(); // TODO
        processExpression(); // TODO
        break;
    case 2: // function call 
        ungetToken(); // TODO
        processFuncCall();
        break;
    default:
        // ERROR
        printf("ERROR processVariableDefStatement - error with assignment\n");
        break;
    } 
}

void processStatement(ast_node *funcDefNode, htab_t **symTable, FILE *f)
{
    // make node for statement 
    ast_node *statementNode = make_new_node();
    token_t token = getToken(f);
    token_t nextToken;
    // detect type of statement 
    switch (token.type)
    {
    case TOKEN_LOCAL:
        // set type of the statement node to variable definition statement
        statementNode->nodeType = NODE_VAR_DEF;
        // process variable definition
        processVariableDefStatement(statementNode, symTable, f); // <statement> --> local id : datatype = <expression_or_func_call>
        break;
    case TOKEN_ID:
        // get next token to detect type of statement
        nextToken = getToken(f);
        if (nextToken.type = TOKEN_COMMA)
        {
            // return token, should be processed again
            ungetToken(); // TODO
            // return token, should be processed again
            ungetToken(); // TODO
            // set type of the node to multiple assignment node
            statementNode->nodeType = NODE_MULTIPLE_ASSIGN;
            // process multiple assignment statement
            processMultipleAssignment(statementNode, symTable, f); // <statement> --> id <more_ids> = <expression_or_func_call>
        }
        else if(nextToken.type = TOKEN_ASSIGNMENT)
        {
            //  return token, should be processed again
            ungetToken(); // TODO
            //  return token, should be processed again
            ungetToken(); // TODO
            // set type of the node to single assignment node
            statementNode->nodeType = NODE_SINGLE_ASSIGN;
            // process single assignment statement
            processSingleAssignment(statementNode, symTable, f); // <statement> --> id = <expression_or_func_call>
        }
        else if(nextToken.type = TOKEN_L_BR)
        {
            //  return token, should be processed again
            ungetToken(); // TODO
            //  return token, should be processed again
            ungetToken(); // TODO
            // set type of the node to function call node
            statementNode->nodeType = NODE_FUNC_CALL;
            // process function call statement 
            processFuncCall(statementNode, symTable, f); // <statement> --> id (<list_of_parameters>)
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
        processIfStatement(statementNode, symTable, f); // <statement> --> if expression then <list_of_statements> else <list_of_statements>  end
        break;
    case TOKEN_WHILE:
        // set type of the statement node to while statement
        statementNode->nodeType = NODE_WHILE;
        // process while statement
        processWhileStatement(statementNode, symTable, f); // <statement> --> while expression do <list_of_statements> end
        break;
    case TOKEN_RET:
        // set type of the statement node to return statement
        statementNode->nodeType = NODE_RETURN;
        // process return statement
        processReturnStatement(statementNode, symTable, f); // <statement> --> return <list_of_expressions>
        break;
    default:
        // ERROR
        printf("ERROR statement - wrong kinf of token\n");
        break;
    }
    // set statement node as a child of function definition node 
    make_new_child(funcDefNode, statementNode);
}

void processDatatypesList(htab_t **symTable, FILE *f, token_t token)
{

}

void processReturnDatatypesList(htab_t **symTable, FILE *f)
{

}

void processParametersList(htab_t **symTable,FILE *f, token_t token)
{

}

// <functions> --> id ( )
void processVoidFunctionCall(ast_node *ast, htab_t **symTable, FILE *f, token_t token)
{
    // make node for function declaration
    ast_node *funcVoidFuncCall = make_new_node();
    funcVoidFuncCall->nodeType = NODE_FUNC_VOID_CALL;
    funcVoidFuncCall->nodeData.stringData = token.data.tokenStringVal;

    token = getToken(f);
    if(token.type != TOKEN_L_BR)
    {
        // ERROR
        printf("ERROR void function call - no ( \n");
    }
    token = getToken(f);
    if(token.type != TOKEN_R_BR)
    {
        // ERROR
        printf("ERROR void function call - no ) \n");
    }
    make_new_child(ast, funcVoidFuncCall);
}

// <functions> --> global id : function ( <list_of_datatypes> ) : <list_of_datatypes>
void processFunctionDeclaration(ast_node *ast, htab_t **symTable, FILE *f)
{
    // make node for function declaration
    ast_node *funcDeclNode = make_new_node();
    funcDeclNode->nodeType = NODE_FUNC_DECL;
    // get next token, should be ID
    token_t token = getToken(f);
    if(token.type != TOKEN_ID)
    {
        // ERROR
        printf("ERROR function declaration - no function ID\n");
    }
    funcDeclNode->nodeData.stringData = token.data.tokenStringVal;
    // TODO add function ID to symtable
    //
    //
    // get next token, should be : 
    token = getToken(f);
    if(token.type != TOKEN_COLON)
    {
        // ERROR
        printf("ERROR function declaration - no : \n");
    }
    // get next token, should be function key word
    token = getToken(f);
    if(token.type != TOKEN_FUNC)
    {
        // ERROR
        printf("ERROR function declaration - no function key word \n");
    }
    // get next token, should be (
    token = getToken(f);
    if(token.type != TOKEN_L_BR)
    {
        // ERROR
        printf("ERROR function declaration - no ( \n");
    }
    // get next token, if it's not ), process list of datatypes
    token = getToken(f);
    if(token.type != TOKEN_R_BR)
    {
        processDatatypesList(symTable, f, token);  // TODO think about function ungetToken(), then there will be one function for datatypes list and return datatypes list
    }
    // get next token, should be :
    token = getToken(f);
    if(token.type != TOKEN_COLON)
    {
        // ERROR
        printf("ERROR function declaration - no : \n");
    }
    // process list of return datatypes 
    processReturnDatatypesList(symTable, f);
    // set funcDef node as a child of program root
    make_new_child(ast, funcDeclNode);
}

// <functions> --> function id ( <list_of_parameters> ) : <list_of_datatypes> <list_of_statements> end
void processFunctionDefinition(ast_node *ast, htab_t **symTable, FILE *f)
{
    // make node for function definition  
    ast_node *funcDefNode = make_new_node();
    funcDefNode->nodeType = NODE_FUNC_DEF;
    //get next token, should be ID
    token_t token = getToken(f);
    if(token.type != TOKEN_ID)
    {
        // ERROR
        printf("ERROR function definition - no function ID\n");
    }
    funcDefNode->nodeData.stringData = token.data.tokenStringVal;
    // TODO add function ID to symtable
    //
    //
    // get next token, should be (
    token = getToken(f);
    if(token.type != TOKEN_L_BR)
    {
        // ERROR
        printf("ERROR function definition - no ( for arguments\n");
    }
    token = getToken(f);
    // if token is not ) process function parameters
    if(token.type != TOKEN_R_BR)
    {
        processParametersList(symTable, f, token);
    }
    // get next token, should be :
    token = getToken(f);
    if(token.type != TOKEN_COLON)
    {
        // ERROR
        printf("ERROR function definition - no : \n");
    }
    // process list of return datatypes 
    processReturnDatatypesList(symTable, f);
    // get next token, if its not END process list of statements
    token = getToken(f);
    while(token.type != TOKEN_END)
    {
        ungetToken();//TODO
        processStatement(funcDefNode, symTable, f);
    }
    // set funcDef node as a child of program root
    make_new_child(ast, funcDefNode);

}

// <prog> --> require "ifj21" <functions> EOF
void processProgramTemplate(ast_node *ast, htab_t **symTable, FILE *f)
{
    // check prolog
    token_t token = getToken(f);
    if(token.type != TOKEN_REQ)
    {
        // ERROR
        printf("ERROR not a require token\n");
    }
    token = getToken(f);
    if(token.type != TOKEN_STR || strcmp(token.data.tokenStringVal, "ifj21") !=0)
    {
        // ERROR
        printf("ERROR not a \"ifj21\" token\n");
    }
    // prolog part is OK!
    // check functions block
    token = getToken(f);
    while(token.type != TOKEN_EOF)
    {
        if(token.type == TOKEN_FUNC) // <functions> --> function id ( <list_of_parameters> ) : <list_of_datatypes> <list_of_statements> end
        {
            processFunctionDefinition(ast, symTable, f);
        }
        else if(token.type == TOKEN_GLOBAL) // <functions> --> global id : function ( <list_of_datatypes> ) : <list_of_datatypes>
        {
            processFunctionDeclaration(ast, symTable, f);
        }
        else if(token.type == TOKEN_ID) // <functions> --> id ( )
        {
            processVoidFunctionCall(ast, symTable, f, token);
        }
        else // don't know this rule :(
        {
            //ERROR
            printf("ERROR not a functions block\n");
        }
        token = getToken(f);
    }

}

// main function that starts building ast
ast_node *parseAST(htab_t **symTable, FILE *f)
{
    ast_node *ast = make_new_node();
    ast->nodeType = NODE_PROG;
    processProgramTemplate(ast, symTable, f);
    return ast;
}



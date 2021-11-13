#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "scanner.h"
#include "dynamic_string.h"
#include "htab.h"



/**
 * @brief make new token
 * 
 * @param f is input file 
 * @return token_t, new token
 */

tokenType_t keywordCheck(char* suspect)
{
    if(!strcmp(suspect, "do")) return TOKEN_DO;
    else if (!strcmp(suspect, "global")) return TOKEN_GLOBAL;
    else if (!strcmp(suspect, "number"))  return TOKEN_NUM_KW;
    else if (!strcmp(suspect, "else")) return TOKEN_ELSE;
    else if (!strcmp(suspect, "if")) return TOKEN_IF;
    else if (!strcmp(suspect, "require")) return TOKEN_REQ;
    else if (!strcmp(suspect, "end")) return TOKEN_END;
    else if (!strcmp(suspect, "integer")) return TOKEN_INT_KW;
    else if (!strcmp(suspect, "return")) return TOKEN_RET;
    else if (!strcmp(suspect, "function")) return TOKEN_FUNC;
    else if (!strcmp(suspect, "local")) return TOKEN_LOCAL;
    else if (!strcmp(suspect, "string")) return TOKEN_STR_KW;
    else if (!strcmp(suspect, "nil")) return TOKEN_NIL;
    else if (!strcmp(suspect, "then")) return TOKEN_THEN;
    else if (!strcmp(suspect, "while")) return TOKEN_WHILE;
    else return 0;
}



token_t getToken(FILE *f)
{
    // dynamic string variable
    DynamicString dynamicString;
    // initialize dynamic string
    DynamicStringInit(&dynamicString);
    // token variable
    token_t newToken;
    newToken.data.tokenStringVal = NULL;
    newToken.data.tokenIntVal = 0;
    newToken.data.tokenNumVal = 0;
    // FSM state variable 
    fsmState currentState = START_STATE;
    // char variable to read symbol from input
    char symbol;
    // read symbol by symbol until it's not a token
    while (1)
    {
        symbol = getc(f);

        switch (currentState) {
            // START_STATE - check next symbol and change state
            case START_STATE:
                if(isspace(symbol))
                {
                    // skip all whitespaces
                    currentState = START_STATE;
                }
                else if(symbol == EOF)
                {
                    // create end of file token
                    newToken.type = TOKEN_EOF;
                    return newToken;
                }
                else if(isalpha(symbol) || symbol == '_')
                {
                    // start of ID or KW token 
                    // add symbol to the string
                    DynamicStringInsertLast(&dynamicString, symbol);
                    //change state to ID_OR_KW_STATE 
                    currentState = ID_OR_KW_STATE;
                }
                else if(isdigit(symbol))
                {
                    // start of the integer token 
                    // add symbol to the string
                    DynamicStringInsertLast(&dynamicString, symbol);
                    // change state to INT_STATE 
                    currentState = INT_STATE;
                }
                else if(symbol == '\"')
                {
                    // start of the string token 
                    // change state to STRING_STATE
                    currentState = STRING_STATE;
                }
                else if(symbol == '~')
                {
                    // start of the not equal token 
                    // change state to PROBABLY_NOT_EQ_STATE
                    currentState = PROBABLY_NOT_EQ_STATE;
                }
                else if(symbol == '=')
                {
                    // start of the equal token 
                    // change state to PROBABLY_EQ_STATE
                    currentState = PROBABLY_EQ_STATE;
                }
                else if(symbol == '>')
                {
                    // start of the greater/greater or equal token 
                    // change state to GREATER_OR_GREATER_OR_EQ_STATE
                    currentState = GREATER_OR_GREATER_OR_EQ_STATE;
                }
                else if(symbol == '<')
                {
                    // start of the less/less or equal token 
                    // change state to LESS_OR_LESS_OR_EQ_STATE
                    currentState = LESS_OR_LESS_OR_EQ_STATE;
                }
                else if(symbol == '.')
                {
                    // start of the concatenation token
                    // change state to DOT_STATE
                    currentState = DOT_STATE;
                }
                else if(symbol == '+')
                {
                    // change state to START_STATE
                    currentState = START_STATE;
                    // create plus token
                    newToken.type = TOKEN_PLUS;
                    return newToken;
                }
                else if(symbol == '/')
                {
                    // start ot div state 
                    currentState = DIV_STATE;
                }
                else if(symbol == '*')
                {
                    // change state to START_STATE
                    currentState = START_STATE;
                    // create mult token 
                    newToken.type = TOKEN_MULT;
                    return newToken;
                }
                else if(symbol == '#')
                {
                    // change state to START_STATE
                    currentState = START_STATE;
                    // create strlen token
                    newToken.type = TOKEN_STRLEN;
                    return newToken;
                }
                else if(symbol == ',')
                {
                    // change state to START_STATE
                    currentState = START_STATE;
                    // create comma token
                    newToken.type = TOKEN_COMMA;
                    return newToken;
                }
                else if(symbol == ':')
                {
                    // change state to START_STATE
                    currentState = START_STATE;
                    // create colon token 
                    newToken.type = TOKEN_COLON;
                    return newToken;
                }
                else if(symbol == '-')
                {
                    // start of comment state or minus token
                    currentState = MINUS_OR_COMMENT_STATE;
                }
                else if(symbol == '(')
                {
                    currentState = START_STATE;
                    newToken.type = TOKEN_L_BR;
                    return newToken;
                }
                else if(symbol == ')')
                {
                    currentState = START_STATE;
                    newToken.type = TOKEN_R_BR;
                    return newToken;
                }
                break;
            
            case ID_OR_KW_STATE:
                if((symbol == '_') || isalpha(symbol) || isdigit(symbol))
                {
                    // add symbol to the string
                    DynamicStringInsertLast(&dynamicString, symbol);
                    // state doesn't change
                    currentState = ID_OR_KW_STATE;
                }
                else
                {
                    // change state to START_STATE
                    currentState = START_STATE;
                    ungetc(symbol,f);
                    // create id token
                    //newToken.type = TOKEN_ID;
                    char* tmp = DynamicStringToString(&dynamicString);
                    newToken.data.tokenStringVal = tmp;
                    free(tmp);
                    newToken.type = keywordCheck(newToken.data.tokenStringVal) ? keywordCheck(newToken.data.tokenStringVal) : TOKEN_ID;
                    return newToken;
                }
                break;

            case INT_STATE:
                if(isdigit(symbol))
                {
                    // add symbol to the string
                    DynamicStringInsertLast(&dynamicString, symbol);
                    // state doesn't change
                    currentState = INT_STATE;
                }
                else if(symbol == '.')
                {
                    // add symbol to the string
                    DynamicStringInsertLast(&dynamicString, symbol);
                    // change state to INT_DOT_STATE
                    currentState = INT_DOT_STATE;
                }
                else if(symbol == 'e' || symbol == 'E')
                {
                    // add symbol to the string 
                    DynamicStringInsertLast(&dynamicString, symbol);
                    // change state to INT_DOT_STATE
                    currentState = EXP_STATE;
                }
                else
                {
                    // return symbol, should be read again
                    ungetc(symbol, f);
                    // change state to START_STATE
                    currentState = START_STATE;
                    // create int token 
                    newToken.type = TOKEN_INT;
                    newToken.data.tokenIntVal = DynamicStringToInt(&dynamicString);
                    return newToken;
                }
                break;

            case INT_DOT_STATE:
                if(isdigit(symbol))
                {
                    // add symbol to the string 
                    DynamicStringInsertLast(&dynamicString, symbol);
                    // change state to NUMBER_STATE
                    currentState = NUMBER_STATE;
                }
                else
                {
                    //TODO - ERROR
                }
                break;

            case NUMBER_STATE:
                if(isdigit(symbol))
                {
                    // add symbol to the string
                    DynamicStringInsertLast(&dynamicString, symbol);
                    // state doesn't change
                    currentState = NUMBER_STATE;
                }
                else if (symbol == 'e' || symbol == 'E')
                {
                    // add symbol to the string
                    DynamicStringInsertLast(&dynamicString, symbol);
                    // change state to EXP_STATE
                    currentState = EXP_STATE;
                }
                else
                {
                    // return symbol, shoud be read again
                    ungetc(symbol, f);
                    // change state to START_STATE
                    currentState = START_STATE;
                    // create num token 
                    newToken.type = TOKEN_NUM;
                    newToken.data.tokenNumVal = DynamicStringToDouble(&dynamicString);
                    return newToken;
                }
                break;

            case EXP_STATE:
                if(symbol == '+' || symbol == '-')
                {
                    // add symbol to the string
                    DynamicStringInsertLast(&dynamicString, symbol);
                    // change state to EXP_PLUS_OR_MINUS_STATE
                    currentState = EXP_PLUS_OR_MINUS_STATE;
                }
                else if (isdigit(symbol))
                {
                    // add symbol to the string
                    DynamicStringInsertLast(&dynamicString, symbol);
                    // change state to EXP_NUMBER_STATE
                    currentState = EXP_NUMBER_STATE;
                }
                else
                {
                    //TODO - maybe ERROR idk...
                }
                break;

            case EXP_PLUS_OR_MINUS_STATE:
                if(isdigit(symbol))
                {
                    // add symbol to the string 
                    DynamicStringInsertLast(&dynamicString, symbol);
                    // change state to EXP_NUMBER_STATE
                    currentState = EXP_NUMBER_STATE;
                }
                else
                {
                    // TODO - ERROR
                }
                break;

            case EXP_NUMBER_STATE:
                if(isdigit(symbol))
                {
                    // add symbol to the string 
                    DynamicStringInsertLast(&dynamicString, symbol);
                    // state doesn't change
                    currentState = EXP_NUMBER_STATE;
                }
                else
                {
                    //return symbol, should be read again
                    ungetc(symbol, f);
                    // change state to START_STATE
                    currentState = START_STATE;
                    // create num token 
                    newToken.type = TOKEN_NUM;
                    newToken.data.tokenNumVal = DynamicStringExpToDouble(&dynamicString);
                    return newToken;
                }
                break;

            case STRING_STATE:
                if(symbol == '\\')
                {
                    currentState = STRING_E_S_STATE;
                }
                else if (symbol == '\"')
                {
                    // change state to START_STATE
                    currentState = START_STATE;
                    newToken.type = TOKEN_STR;
                    newToken.data.tokenStringVal = DynamicStringToString(&dynamicString);
                    return newToken;
                }
                else
                {
                    // add symbol to the string
                    DynamicStringInsertLast(&dynamicString, symbol);
                    // state doesn't change 
                    currentState = STRING_STATE;
                }
                break;

            case STRING_E_S_STATE:
                switch (symbol)
                {
                case '\\':
                    // add symbol to the string
                    DynamicStringInsertLast(&dynamicString, '\\');
                    currentState = STRING_STATE;
                    break;
                case '\"':
                    // add symbol to the string
                    DynamicStringInsertLast(&dynamicString, '\"');
                    currentState = STRING_STATE;
                    break;
                case 't':
                    // add symbol to the string
                    DynamicStringInsertLast(&dynamicString, '\t');
                    currentState = STRING_STATE;
                    break;
                case 'n':
                    // add symbol to the string
                    DynamicStringInsertLast(&dynamicString, '\n');
                    currentState = STRING_STATE;
                    break;
                case '0':
                    escape_sequence[0] = symbol;
                    currentState = STRING_E_S_0_1_STATE;
                    break;
                case '1':
                    escape_sequence[0] = symbol;
                    currentState = STRING_E_S_0_1_STATE;
                    break;
                case '2':
                    escape_sequence[0] = symbol;
                    currentState = STRING_E_S_2_STATE;
                    break;
                default:
                    break;
                }
                break;
            
            case STRING_E_S_0_1_STATE:
                if(isdigit(symbol))
                {
                    escape_sequence[1] = symbol;
                    symbol = getc(f);
                    if(isdigit(symbol))
                    {
                        escape_sequence[2] = symbol;
                        DynamicStringInsertLast(&dynamicString, atoi(escape_sequence));
                        currentState = STRING_STATE;
                    }
                    else
                    {
                        // ERROR
                    }
                }
                else
                {
                    //ERROR
                }
                break;
            
            case STRING_E_S_2_STATE:
                if(symbol >= '0' && symbol <='5')
                {
                    escape_sequence[1] = symbol;
                    symbol = getc(f);
                    if(symbol >= '0' && symbol <='5')
                    {
                        escape_sequence[2] = symbol;
                        DynamicStringInsertLast(&dynamicString, atoi(escape_sequence));
                        currentState = STRING_STATE;
                    }
                    else
                    {
                        // ERROR
                    }
                }
                else
                {
                    // ERROR
                }
                break;

            case DIV_STATE:
                if(symbol == '/')
                {
                    // change state to START_STATE
                    currentState = START_STATE;
                    // create token int div
                    newToken.type = TOKEN_D_BS;
                    return newToken;
                }
                else
                {
                    // return symbol, should be read again
                    ungetc(symbol,f);
                    // change state to START_STATE
                    currentState = START_STATE;
                    // create div token
                    newToken.type = TOKEN_S_BS;
                    return newToken;
                }
                break;

            case PROBABLY_NOT_EQ_STATE:
                if(symbol == '=')
                {
                    // change state to START_STATE
                    currentState = START_STATE;
                    // create not equal token 
                    newToken.type = TOKEN_NEQ;
                    return newToken;
                }
                else
                {
                    //TODO ERROR
                }
                break;

            case PROBABLY_EQ_STATE:
                if(symbol == '=')
                {
                    // change state to START_STATE
                    currentState = START_STATE;
                    // create equal token
                    newToken.type = TOKEN_EQ;
                    return newToken;
                }
                else
                {
                    ungetc(symbol,f);
                    currentState = START_STATE;
                    newToken.type = TOKEN_ASSIGNMENT;
                    return newToken;
                }
                break;

            case GREATER_OR_GREATER_OR_EQ_STATE:
                if(symbol == '=')
                {
                    // change state to START_STATE
                    currentState = START_STATE;
                    // create greater or equal token
                    newToken.type = TOKEN_GREAT_EQ;
                    return newToken;
                }
                else
                {
                    // change state to START_STATE
                    currentState = START_STATE;
                    // return symbol, should be read again
                    ungetc(symbol, f);
                    // create greater token
                    newToken.type = TOKEN_GREAT;
                    return newToken;
                }
                break;

            case LESS_OR_LESS_OR_EQ_STATE:
                if(symbol == '=')
                {
                    // change state to START_STATE
                    currentState = START_STATE;
                    // create less or equal token 
                    newToken.type = TOKEN_LESS_EQ;
                    return newToken;
                }
                else
                {
                    // change state to START_STATE
                    currentState = START_STATE;
                    // return symbol, should be read again
                    ungetc(symbol, f);
                    // create less token
                    newToken.type = TOKEN_LESS;
                    return newToken;
                }
                break;

            case DOT_STATE:
                if(symbol == '.')
                {
                    // change state to START_STATE
                    currentState = START_STATE;
                    // create concatenation token 
                    newToken.type = TOKEN_CONC;
                    return newToken;
                }
                else
                {
                    //TODO - ERROR
                }
                break;

            case MINUS_OR_COMMENT_STATE:
                if(symbol == '-')
                {
                    // change state to LINE_COMMENT_STATE
                    currentState = LINE_COMMENT_STATE;
                }
                else
                {
                    //change state to START_STATE
                    currentState = START_STATE;
                    // return symbol, should be read again
                    ungetc(symbol, f);
                    // create minus token 
                    newToken.type = TOKEN_MINUS;
                    return newToken;
                }
                break;

            case LINE_COMMENT_STATE:
                if(symbol == '[')
                {
                    // channge state to PROBABLY_BLOCK_COMMENT_STATE
                    currentState = PROBABLY_BLOCK_COMMENT_STATE;
                }
                else if (symbol == '\n' || symbol == '\r')
                {
                    // change state to START_STATE
                    currentState = START_STATE;
                }
                else
                {
                    // change state to LINE_COMMENT_STATE
                    currentState = LINE_COMMENT_STATE;
                }
                break;

            case PROBABLY_BLOCK_COMMENT_STATE:
                if(symbol == '[')
                {
                    // change state to BLOCK_COMMENT_STATE
                    currentState = BLOCK_COMMENT_STATE;
                }
                else
                {
                    // change state to LINE_COMMENT_STATE
                    currentState = LINE_COMMENT_STATE;
                }
                break;

            case BLOCK_COMMENT_STATE:
                if(symbol == ']')
                {
                    // change state to BLOCK_COMMENT_END_STATE
                    currentState = BLOCK_COMMENT_END_STATE;
                }
                else
                {
                    // change state to BLOCK_COMMENT_STATE
                    currentState = BLOCK_COMMENT_STATE;
                }
                break;

            case BLOCK_COMMENT_END_STATE:
                if(symbol == ']')
                {
                    // change state to START_STATE
                    currentState = START_STATE;
                }
                else
                {
                    // change state to BLOCK_COMMENT_STATE
                    currentState = BLOCK_COMMENT_STATE;
                }
                break;
        }
    }
    return newToken;
}



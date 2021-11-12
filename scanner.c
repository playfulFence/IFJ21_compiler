#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "scanner.h"

token_t getToken(FILE *f)
{
    DynamicString *dynamicString;
    DynamicStringInit(dynamicString); 
    token_t newToken;
    fsmState currentState = START_STATE;
    char symbol;

    while (1)
    {
        symbol = getc(f);
        switch (currentState) {
            case START_STATE:
                switch (symbol)
                {
                    case ' ': // white space
                    case '\t': // tab
                    case '\n': // newline
                    case '\r': // other newline
                        currentState = START_STATE;
                        break;
                    case EOF:
                        newToken.type = TOKEN_EOF;
                        return newToken;
                    case '_':
                    case 'a'...'z':
                    case 'A'...'Z':
                        // TODO add symbol to dynamic string
                        DynamicStringInsertLast(dynamicString, symbol);
                        currentState = ID_OR_KW_STATE;
                        break;
                    case '0' ... '9':
                        //TODO - add symbol to string
                        DynamicStringInsertLast(dynamicString, symbol);
                        currentState = INT_STATE;
                        break;
                    case '\"':
                        currentState = STRING_STATE;
                        break;
                    case '~':
                        currentState = PROBABLY_NOT_EQ_STATE;
                        break;
                    case '=':
                        currentState = PROBABLY_EQ_STATE;
                        break;
                    case '>':
                        currentState = GREATER_OR_GREATER_OR_EQ_STATE;
                        break;
                    case '<':
                        currentState = LESS_OR_LESS_OR_EQ_STATE;
                        break;
                    case '.':
                        currentState = DOT_STATE;
                        break;
                    case '+':
                        //TODO - process token
                        currentState = START_STATE;
                        newToken.type = TOKEN_PLUS;
                        return newToken;
                    case '/':
                        //TODO - process  token
                        currentState = DIV_STATE;
                        break;
                    case '*':
                        //TODO - process  token
                        currentState = START_STATE;
                        newToken.type = TOKEN_MULT;
                        return newToken;
                    case '#':
                        //TODO - process  token
                        currentState = START_STATE;
                        newToken.type = TOKEN_STRLEN;
                        return newToken;
                    case ',':
                        //TODO - process  token
                        currentState = START_STATE;
                        newToken.type = TOKEN_COMMA;
                        return newToken;
                    case ':':
                        //TODO - process  token
                        currentState = START_STATE;
                        newToken.type = TOKEN_COLON;
                        return newToken;
                    case '-':
                        currentState = MINUS_OR_COMMENT_STATE;
                        break;
                    default:
                        break;
                }
                break;
            case ID_OR_KW_STATE:
                if((symbol == '_') ||
                   (symbol >= 'a' && symbol <= 'z') ||
                   (symbol >= 'A' && symbol <= 'Z') ||
                   (symbol >= '0' && symbol <= '9'))
                {
                    //TODO - add symbol to string
                    DynamicStringInsertLast(dynamicString, symbol);
                    currentState = ID_OR_KW_STATE;
                }
                else
                {
                    //TODO - make a string from dynamic string and return a token
                    currentState = START_STATE;
                    newToken.type = TOKEN_ID;
                    newToken.data.tokenStringVal = DynamicStringToString(dynamicString);
                    return newToken;
                }
                break;
            case INT_STATE:
                if(symbol >= '0' && symbol <= '9')
                {
                    //TODO - add symbol to string
                    DynamicStringInsertLast(dynamicString, symbol);
                    currentState = INT_STATE;
                }
                else if(symbol == '.')
                {
                    //TODO - add symbol to the string
                    DynamicStringInsertLast(dynamicString, symbol);
                    currentState = INT_DOT_STATE;
                }
                else if(symbol == 'e' || symbol == 'E')
                {
                    //TODO - add symbol to the string
                    DynamicStringInsertLast(dynamicString, symbol);
                    currentState = EXP_STATE;
                }
                else
                {
                    //TODO - return symbol, should be read again
                    ungetc(symbol, f);
                    currentState = START_STATE;
                    newToken.type = TOKEN_INT;
                    newToken.data.tokenIntVal = DynamicStringToInt(dynamicString);
                    return newToken;
                }
                break;
            case INT_DOT_STATE:
                if(symbol >= '0' && symbol <= '9')
                {
                    //TODO - add symbol to the string
                    DynamicStringInsertLast(dynamicString, symbol);
                    currentState = NUMBER_STATE;
                }
                else
                {
                    //TODO - ERROR
                }
                break;
            case NUMBER_STATE:
                if(symbol >= '0' && symbol <= '9')
                {
                    //TODO - add symbol to the string
                    DynamicStringInsertLast(dynamicString, symbol);
                    currentState = NUMBER_STATE;
                }
                else if (symbol == 'e' || symbol == 'E')
                {
                    //TODO - add symbol to the string 
                    DynamicStringInsertLast(dynamicString, symbol);
                    currentState = EXP_STATE;
                }
                else
                {
                    // TODO - return symbol, should be read again
                    ungetc(symbol, f);
                    currentState = START_STATE;
                    newToken.type = TOKEN_NUM;
                    newToken.data.tokenNumVal = DynamicStringToDouble(dynamicString);
                    return newToken;
                }
                break;
            case EXP_STATE:
                if(symbol == '+' || symbol == '-')
                {
                    //TODO - add symbol to the string
                    DynamicStringInsertLast(dynamicString, symbol);
                    currentState = EXP_PLUS_OR_MINUS_STATE;
                }
                else if (symbol >= '0' && symbol <= '9')
                {
                    //TODO - add symbol to the string
                    DynamicStringInsertLast(dynamicString, symbol);
                    currentState = EXP_NUMBER_STATE;
                }
                else
                {
                    //TODO - maybe ERROR idk...
                }
                break;
            case EXP_PLUS_OR_MINUS_STATE:
                if(symbol >= '0' && symbol <= '9')
                {
                    // TODO - add symbol to the string
                    DynamicStringInsertLast(dynamicString, symbol);
                    currentState = EXP_NUMBER_STATE;
                }
                else
                {
                    // TODO - ERROR
                }
                break;
            case EXP_NUMBER_STATE:
                if(symbol >= '0' && symbol <= '9')
                {
                    // TODO - add symbol to the string
                    DynamicStringInsertLast(dynamicString, symbol);
                    currentState = EXP_NUMBER_STATE;
                }
                else
                {
                    // TODO - return symbol
                    // TODO - process token 
                    ungetc(symbol, f);
                    currentState = START_STATE;
                    newToken.type = TOKEN_NUM;
                    newToken.data.tokenNumVal = DynamicStringExpToDouble(dynamicString);
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
                    currentState = STRING_END_STATE;
                }
                else
                {
                    //TODO - add symbol to the string
                    DynamicStringInsertLast(dynamicString, symbol);
                    currentState = STRING_STATE;
                }
                break;
            case STRING_E_S_STATE:
                switch (symbol)
                {
                case '\\':
                    //TODO - add symbol to the string
                    DynamicStringInsertLast(dynamicString, '\\');
                    currentState = STRING_STATE;
                    break;
                case '\"':
                    //TODO - add symbol to the string
                    DynamicStringInsertLast(dynamicString, '\"');
                    currentState = STRING_STATE;
                    break;
                case 't':
                    //TODO - add symbol to the string
                    DynamicStringInsertLast(dynamicString, '\t');
                    currentState = STRING_STATE;
                    break;
                case 'n':
                    //TODO - add symbol to the string
                    DynamicStringInsertLast(dynamicString, '\n');
                    currentState = STRING_STATE;
                    break;
                case '0':
                    //TODO
                    break;
                case '1':
                    //TODO
                    break;
                case '2':
                    //TODO
                    break;
                default:
                    break;
                }
            case STRING_END_STATE:
                //TODO - process string
                currentState = START_STATE;
                break;
            case DIV_STATE:
                if(symbol == '/')
                {
                    newToken.type = TOKEN_D_BS;
                    return newToken;
                }
                else
                {
                    ungetc(symbol,f);
                    newToken.type = TOKEN_S_BS;
                    return newToken;
                }
            case PROBABLY_NOT_EQ_STATE:
                if(symbol == '=')
                {
                    //TODO - process token
                    currentState = START_STATE;
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
                    //TODO - process token
                    currentState = START_STATE;
                    newToken.type = TOKEN_EQ;
                }
                else
                {
                    //TODO - ERROR
                }
                break;
            case GREATER_OR_GREATER_OR_EQ_STATE:
                if(symbol == '=')
                {
                    //TODO - process token
                    currentState = START_STATE;
                    newToken.type = TOKEN_GREAT_EQ;
                    return newToken;
                }
                else
                {
                    //TODO - return symbol, should be read again
                    //TODO - process token
                    currentState = START_STATE;
                    ungetc(symbol, f);
                    newToken.type = TOKEN_GREAT;
                    return newToken;
                }
                break;
            case LESS_OR_LESS_OR_EQ_STATE:
                if(symbol == '=')
                {
                    //TODO - process token
                    currentState = START_STATE;
                    newToken.type = TOKEN_LESS_EQ;
                    return newToken;
                }
                else
                {
                    //TODO - return symbol, should be read again
                    //TODO - process token
                    currentState = START_STATE;
                    ungetc(symbol, f);
                    newToken.type = TOKEN_LESS;
                }
                break;
            case DOT_STATE:
                if(symbol == '.')
                {
                    //TODO - process token
                    currentState = START_STATE;
                    newToken.type = TOKEN_CONC;
                    return newToken;
                }
                else
                {
                    //TODO - ERROR
                }
            case MINUS_OR_COMMENT_STATE:
                if(symbol == '-')
                {
                    currentState = LINE_COMMENT_STATE;
                }
                else
                {
                    //TODO - process token
                    //TODO - return symbol, should be read again
                    currentState = START_STATE;
                    ungetc(symbol, f);
                    newToken.type = TOKEN_MINUS;
                }
                break;
            case LINE_COMMENT_STATE:
                if(symbol == '[')
                {
                    currentState = PROBABLY_BLOCK_COMMENT_STATE;
                }
                else if (symbol == '\n' || symbol == '\r')
                {
                    currentState = START_STATE;
                }
                else
                {
                    currentState = LINE_COMMENT_STATE;
                }
                break;
            case PROBABLY_BLOCK_COMMENT_STATE:
                if(symbol == '[')
                {
                    currentState = BLOCK_COMMENT_STATE;
                }
                else
                {
                    currentState = LINE_COMMENT_STATE;
                }
                break;
            case BLOCK_COMMENT_STATE:
                if(symbol == ']')
                {
                    currentState = BLOCK_COMMENT_END_STATE;
                }
                else
                {
                    currentState = BLOCK_COMMENT_STATE;
                }
                break;
            case BLOCK_COMMENT_END_STATE:
                if(symbol == ']')
                {
                    currentState = START_STATE;
                }
                else
                {
                    currentState = BLOCK_COMMENT_STATE;
                }
                break;
        }
    }
    return newToken;
}



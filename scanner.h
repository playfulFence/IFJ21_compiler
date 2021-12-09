// Authors : Maksim Naumenko(xnaume01)
        //   Kirill Mikhailov(xmikha00)

//Projekt: Implementace překladače imperativního jazyka IFJ21



#ifndef __SCANNER_H__
#define __SCANNER_H__

#include <stdio.h>
#include "stack_p_t.h"
#include "dynamic_string.h"
#include "token.h"

typedef enum fsmStates
{
    START_STATE,                    // start state 
    ID_OR_KW_STATE,                 // identifier or key word state
    INT_STATE,                      // integer state 
    INT_DOT_STATE,                  // got dot after integer (123.), should be number token
    EXP_STATE,                      // got e/E after integer (123e/E), should be number token
    EXP_PLUS_OR_MINUS_STATE,        // got +/- after integer with e/E (123e/E+/-), should be number token
    EXP_NUMBER_STATE,               // got number with exponent
    NUMBER_STATE,                   // number state 
    STRING_STATE,                   // string state
    STRING_E_S_STATE,               // escape sequence state
    STRING_E_S_0_1_STATE,           // got \0 or \1 in escape sequence
    STRING_E_S_2_STATE,             // got \2 in escape sequence 
    DIV_STATE,                      // division state
    DOT_STATE,                      // concatenation state
    PROBABLY_NOT_EQ_STATE,          // not equal state
    LINE_COMMENT_STATE,             // line comment state 
    PROBABLY_BLOCK_COMMENT_STATE,   // check if it's block comment 
    BLOCK_COMMENT_STATE,            // block comment state
    BLOCK_COMMENT_END_STATE,        // end of block comment
    PROBABLY_EQ_STATE,              // equal state
    GREATER_OR_GREATER_OR_EQ_STATE, // greater or greater or equal state
    LESS_OR_LESS_OR_EQ_STATE,       // less or less or equal state
    MINUS_OR_COMMENT_STATE          // minus or comment state
}fsmState;

token_t *getToken(FILE *f, DynamicString *dynamicString, StackTokens *tokenStack);

void ungetToken(token_t *token, StackTokens *tokenStack);

char escape_sequence[3];  // variable to process escape sequence 

#endif // __SCANNER_H__

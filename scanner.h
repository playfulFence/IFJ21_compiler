#ifndef __SCANNER_H__
#define __SCANNER_H__

#include <stdio.h>

typedef enum tokenType
{
    TOKEN_ID,           // identifier token
    TOKEN_COLON,        // colon token            (:)
    TOKEN_COMMA,        // comma token            (,)
    TOKEN_INT,          // "int" datatype token
    TOKEN_NUM,          // "num" datatype token
    TOKEN_STR,          // "string" datatype token
    TOKEN_WHILE,        // "while" keyword token
    TOKEN_DO,           // "do" keyword token
    TOKEN_IF,           // "if" condition token
    TOKEN_THEN,         // "then" keyword token
    TOKEN_ELSE,         // "else" keyword token
    TOKEN_END,          // "end" keyword token
    TOKEN_NUM_KW,       // "number" keyword token
    TOKEN_INT_KW,       // "integer" keyword token
    TOKEN_STR_KW,    // "string" keyword token
    TOKEN_FUNC,         // function token
    TOKEN_GLOBAL,       // "global" token         (global)
    TOKEN_LOCAL,        // "local" token          (local)
    TOKEN_NIL,          // nil(NULL) token        (nil)
    TOKEN_REQ,          // require token          (require)
    TOKEN_RET,          // return token           (ret)
    TOKEN_STRLEN,       // "strlen" operand token (#)
    TOKEN_PLUS,         // plus token             (+)
    TOKEN_MINUS,        // minus token            (-)
    TOKEN_MULT,         // multiply token         (*)
    TOKEN_S_BS,         // single backslash token (/)
    TOKEN_D_BS,         // double backslash token (//)
    TOKEN_CONC,         // concatenation token    (..)
    TOKEN_LESS,         // less token             (<)
    TOKEN_GREAT,        // greater token          (>)
    TOKEN_LESS_EQ,      // less or equal token    (<=)
    TOKEN_GREAT_EQ,     // greater or equal token (>=)
    TOKEN_EQ,           // equal token            (==)
    TOKEN_ASSIGNMENT,   // assignment token       (=)
    TOKEN_NEQ,          // not equal token        (~=)
    TOKEN_L_BR,         // left bracket token     ("(")
    TOKEN_R_BR,         // right bracket token    (")")
    TOKEN_EOF           // EOF token
}tokenType_t;

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

typedef struct tokenData
{
    int tokenIntVal;
    char* tokenStringVal;
    double tokenNumVal;
} tokenData_t;

typedef struct token
{
    tokenType_t type;
    tokenData_t data;
}token_t;

token_t getToken(FILE *f);

char escape_sequence[3];  // variable to process escape sequence 

#endif // __SCANNER_H__

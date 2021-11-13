#ifndef __SCANNER_H__
#define __SCANNER_H__

#include <stdio.h>
#include "dynamic_string.h"

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
    TOKEN_EQ,           // equal token            (=)
    TOKEN_NEQ,          // not equal token        (~=)
    TOKEN_L_BR,         // left bracket token     ("(")
    TOKEN_R_BR,         // right bracket token    (")")
    TOKEN_EOF           // EOF token
}tokenType_t;

typedef enum fsmStates
{
    START_STATE, 
    ID_OR_KW_STATE,

    INT_STATE,
    INT_DOT_STATE,
    EXP_STATE,
    EXP_PLUS_OR_MINUS_STATE,
    EXP_NUMBER_STATE,
    NUMBER_STATE,

    STRING_STATE,
    STRING_E_S_STATE,
    DIV_STATE,
    DOT_STATE,
    PROBABLY_NOT_EQ_STATE,
    LINE_COMMENT_STATE,
    PROBABLY_BLOCK_COMMENT_STATE,
    BLOCK_COMMENT_STATE,
    BLOCK_COMMENT_END_STATE,
    PROBABLY_EQ_STATE,
    GREATER_OR_GREATER_OR_EQ_STATE,
    LESS_OR_LESS_OR_EQ_STATE,
    MINUS_OR_COMMENT_STATE
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

#endif // __SCANNER_H__

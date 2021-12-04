#ifndef __TOKEN_H__
#define __TOKEN_H__

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
    TOKEN_STR_KW,       // "string" keyword token
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

typedef struct tokenData
{
    int tokenIntVal;
    char* tokenStringVal;
    double tokenNumVal;
} tokenData_t;

typedef struct token
{ 
    /* Kirill */
    int line;
    /* Kirill */
    tokenType_t type;
    tokenData_t data;
}token_t;

#endif // __TOKEN_H__

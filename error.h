#ifndef __ERROR_LIB_H__
#define __ERROR_LIB_H__


typedef enum 
{
/*1*/  BAD_LEXEM_STRUCT_ERR = 1,
/*2*/  BAD_SYNTAX_ERR,                   // bad program syntax
/*3*/  SEMANTIC_UNDEF_REDEF_ERR,         // undefined function/variable, redefinition and so on
/*4*/  SEMANTIC_ASSIGN_ERR,              // type incompatibility in assign operation
/*5*/  SEMANTIC_PARAM_COUNT_ERR,         // bad count/type of parameters
/*6*/  SEMANTIC_IN_EXPRESSION_TYPES_ERR, // incompatibility of types in expression(arithmetics, string and so on)
/*7*/  SEMANTIC_ANOTHER_ERR,             // another semantic errors
/*8*/  UNEXPECTED_NIL_ERR,               // error in work with unexpected NIL-value
/*9*/  ZERO_DIV_ERR,                     // divide by zero error
/*99*/ COMPILER_INTERN_ERR = 99          // intern error inside of compiler itself(memory allocation error and so on)
}errorType_t;

void errorExit(errorType_t, int);

#endif // __ERROR_LIB_H__

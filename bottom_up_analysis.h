#ifndef __BOTTOM_UP_ANALYSIS__
#define  __BOTTOM_UP_ANALYSIS__

#include "scanner.h"
#include "stack_p_t.h"
#include "stack_p_t_elements.h"
#include "dynamic_string.h"

// precedence table for bottom-up analysis 
// 0 - no rule
// 1 - >
// 2 - < 
// 3 - =
int precedenceTable[17][17] =
{         /*  #   *   /  //   +   -  ..   <  <=   >  >=  ==  ~=   (   )   i   $   */
/* #  */    { 0 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 2 , 0 , 2 , 1 },
/* *  */    { 2 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 2 , 1 , 2 , 1 },
/* /  */    { 2 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 2 , 1 , 2 , 1 },
/* // */    { 2 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 2 , 1 , 2 , 1 },
/* +  */    { 2 , 2 , 2 , 2 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 2 , 1 , 2 , 1 },
/* -  */    { 2 , 2 , 2 , 2 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 2 , 1 , 2 , 1 },
/* .. */    { 2 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 1 , 1 , 1 , 1 , 1 , 2 , 1 , 2 , 1 },
/* <  */    { 2 , 2 , 2 , 2 , 2 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 2 , 1 , 2 , 1 },
/* <= */    { 2 , 2 , 2 , 2 , 2 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 2 , 1 , 2 , 1 },
/* >  */    { 2 , 2 , 2 , 2 , 2 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 2 , 1 , 2 , 1 },
/* >= */    { 2 , 2 , 2 , 2 , 2 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 2 , 1 , 2 , 1 },
/* == */    { 2 , 2 , 2 , 2 , 2 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 2 , 1 , 2 , 1 },
/* ~= */    { 2 , 2 , 2 , 2 , 2 , 2 , 2 , 0 , 0 , 0 , 0 , 0 , 0 , 2 , 1 , 2 , 1 },
/* (  */    { 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 3 , 2 , 0 },
/* )  */    { 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 0 , 1 , 0 , 1 },
/* i  */    { 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 0 , 1 , 0 , 1 },
/* $  */    { 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 0 , 2 , 0 },
};

// rules for bottom-up analysis
typedef enum ptRulese
{
    ID,          // E --> id
    STRLEN_E,    // E --> #E
    E_MULT_E,    // E --> E*E
    E_DIV_E,     // E --> E/E
    E_INTDIV_E,  // E --> E//E
    E_PLUS_E,    // E --> E+E
    E_MINUS_E,   // E --> E-E
    E_CONC_E,    // E --> E..E
    E_LESS_E,    // E --> E<E
    E_LEQ_E,     // E --> E<=E
    E_GREATER_E, // E --> E>E
    E_GEQ_E,     // E --> E>=E
    E_EQ_E,      // E --> E==E
    E_NEQ_E,     // E --> E~=E
    LB_E_RB,     // E --> (E)
    ERROR        // Ooops, I'm sorry but I don't know this rule:(
} ptRule;

// elements that are used for bottom-up analysis
typedef enum ptElements
{
    STRLEN,
    MULT,
    DIV,
    INTDIV,
    PLUS,
    MINUS,
    CONC,
    LESS,
    LEQ,
    GREATER,
    GEQ,
    EQ,
    NEQ,
    LB,
    RB,
    I,
    DOLLAR,
    REDUCE_FLAG,
    E
} ptElement;

// function that transform token type to precedece table element
ptElement ptFromTokenToPTElement(tokenType_t);

// function that get index of precedence table element
int ptGetIndex(ptElement);

// main function for bottom-up analysis 
token_t bottomUpAnalysis(token_t);

#endif // __BOTTOM_UP_ANALYSIS__

#ifndef __DYNAMIC_STRING_H__
#define  __DYNAMIC_STRING_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Symbol
{
    char symbol;
    struct Symbol *nextSymbol;
} *SymbolPtr;

typedef struct 
{
    SymbolPtr firstSymbol;
    SymbolPtr lastSymbol;
    int size;
    //SymbolPtr activeSymbol;
} DynamicString;

void DynamicStringInit(DynamicString *);
void DynamicStringDispose(DynamicString *);
void DynamicStringInsertLast(DynamicString *, char);
char *DynamicStringToString(DynamicString *);
int DynamicStringToInt(DynamicString *);
double DynamicStringToDouble(DynamicString *);
double DynamicStringExpToDouble(DynamicString *);

#endif 

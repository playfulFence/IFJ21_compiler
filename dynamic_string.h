//Author: Maksim Naumenko(xnaume01)


//Projekt: Implementace překladače imperativního jazyka IFJ21


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

void DynamicStringInit(DynamicString * dynamicString);
void DynamicStringDispose(DynamicString * dynamicString);
void DynamicStringInsertLast(DynamicString * dynamicString, char symbol);
char *DynamicStringToString(DynamicString * dynamicString);
int DynamicStringToInt(DynamicString * dynamicString);
double DynamicStringToDouble(DynamicString * dynamicString);
double DynamicStringExpToDouble(DynamicString * dynamicString);
void printDynamicString(DynamicString *);
void DynamicStringReverse(DynamicString *, DynamicString *);

#endif 

#include "dynamic_string.h"

void DynamicStringInit(DynamicString *dynamicString)
{
    dynamicString->firstSymbol = NULL;
    dynamicString->lastSymbol = NULL;
    dynamicString->size = 0;
}

void DynamicStringDispose(DynamicString *dynamicString)
{
    SymbolPtr ephemera;
    while (dynamicString->firstSymbol != NULL && dynamicString->lastSymbol != NULL)
    {
        ephemera = dynamicString->firstSymbol;
        dynamicString->firstSymbol = ephemera->nextSymbol;
        free(ephemera);
    }
    dynamicString->firstSymbol = NULL;
    dynamicString->lastSymbol = NULL;
    dynamicString->size = 0;

}

void DynamicStringInsertLast(DynamicString *dynamicString, char symbol)
{
    SymbolPtr ephemera = (SymbolPtr)malloc(sizeof(struct Symbol));
    if(ephemera == NULL)
    {
        exit(EXIT_FAILURE);
    }
    ephemera->symbol = symbol;
    ephemera->nextSymbol = NULL;
    if(dynamicString->firstSymbol == NULL && dynamicString->lastSymbol == NULL)
    {
        dynamicString->firstSymbol = ephemera;
    }
    else
    {
        dynamicString->lastSymbol->nextSymbol = ephemera;
    }
    dynamicString->lastSymbol = ephemera;
    dynamicString->size++;
}

void DynamicStringInsertFirst(DynamicString *dynamicString, char symbol)
{
    SymbolPtr ephemera = (SymbolPtr)malloc(sizeof(struct Symbol));
    if(ephemera == NULL)
    {
        exit(EXIT_FAILURE);
    }
    ephemera->symbol = symbol;
    ephemera->nextSymbol = NULL;
    if(dynamicString->firstSymbol == NULL && dynamicString->lastSymbol == NULL)
    {
        dynamicString->firstSymbol = ephemera;
        dynamicString->lastSymbol = ephemera;
    }
    else
    {
        ephemera->nextSymbol = dynamicString->firstSymbol;
        dynamicString->firstSymbol = ephemera;
    }
    dynamicString->size++;
}

void DynamicStringReverse(DynamicString *originalString, DynamicString *reversedString)
{
    SymbolPtr ephemera = originalString->firstSymbol;
    while(ephemera != NULL)
    {
        DynamicStringInsertFirst(reversedString, ephemera->symbol);
        ephemera = ephemera->nextSymbol;
    }
}

char *DynamicStringToString(DynamicString *dynamicString)
{
    int size = dynamicString->size;
    int i = 0;
    char *string = malloc(size*sizeof(char)); // +1 не точно!!!

    SymbolPtr ephemera = dynamicString->firstSymbol;
    while(ephemera != NULL)
    {
        string[i] = ephemera->symbol;
        ephemera = ephemera->nextSymbol;
        i++;
    }
    DynamicStringDispose(dynamicString);
    return string;
}

int DynamicStringToInt(DynamicString *dynamicString)
{
    char *string = DynamicStringToString(dynamicString);
    int number = atoi(string);
    return number;
}

double DynamicStringToDouble(DynamicString *dynamicString)
{
    char *string = DynamicStringToString(dynamicString);
    double number = strtod(string, NULL);
    return number;
}

double DynamicStringExpToDouble(DynamicString *dynamicString)
{
    char *string = DynamicStringToString(dynamicString);
    double number = atof(string);
    return number;
}

void printDynamicString(DynamicString *dynamicString)
{
    SymbolPtr pathfinder = dynamicString->firstSymbol;
    printf(".........DYNAMIC STRING.........\n");
    printf("START ");
    while(pathfinder->nextSymbol != NULL)
    {
        printf("[ %c ]-->", pathfinder->symbol);
        pathfinder = pathfinder->nextSymbol;
    }
    printf("[ %c ] END\n", pathfinder->symbol);
}

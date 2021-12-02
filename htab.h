#ifndef __HTAB_H__
#define __HTAB_H__

#include <string.h>     // size_t
#include <stdbool.h>    // bool


#define FROM_FIRST 0    // For listSearch function
#define FROM_SECOND 1


typedef struct htab_item htab_item_t;

typedef struct htab htab_t;

// Typy:
typedef const char * htab_key_t;        // typ klíče


typedef enum
{
    TYPE_STRING,
    TYPE_INT,
    TYPE_NUM,
    TYPE_FUNC,
    TYPE_NIL,
    TYPE_VARIABLE
}elementType_t;

typedef enum
{
    DATATYPE_INT,
    DATATYPE_NUM,
    DATATYPE_STRING,
    DATATYPE_NIL
}variableDatatype_t;

// Tabulka:
struct htab
{
    int size;  /* actual count of records in table, will be (inc)/(dec)remented if we gonna add/delete element from table */
    size_t arr_size;    /* Constant number of pointers to elements (will be same with number in htab_init function) */
    htab_item_t* itemPtrsArr[];
};


// Dvojice dat v tabulce:
typedef struct htab_data {
    bool defineFlag;
    bool declareFlag;
    htab_key_t key;          //  name of variable/function
    elementType_t type;
    variableDatatype_t datatype;    // datatype of variable
    int varIntVal;
    double varNumVal;
    char* varStrVal;
    int countOfArgs;
    int countOfReturns;
    struct htab_data** funcArgs;     // array of function arguments
    struct htab_data** funcReturns;  // array of function return variables
} htab_data_t;


struct htab_item
{
    htab_item_t* next;
    htab_data_t *itemData;
};

typedef struct htab_list_item // for "scope table"
{
    htab_t* symtable;
    struct htab_list_item* next;
}htab_list_item_t;

typedef struct htab_list
{
    htab_list_item_t* first;
}htab_list_t;



// Rozptylovací (hash) funkce (stejná pro všechny tabulky v programu)
// Pokud si v programu definujete stejnou funkci, použije se ta vaše.
size_t htab_hash_function(htab_key_t str);

// Funkce pro práci s tabulkou:
htab_t *htab_init(size_t n);                    // konstruktor tabulky
htab_t *htab_move(size_t n, htab_t *from);      // přesun dat do nové tabulky TODO mb delete
size_t htab_size(const htab_t * t);             // počet záznamů v tabulce
size_t htab_bucket_count(const htab_t * t);     // velikost pole

htab_data_t *htab_find(htab_t * t, htab_key_t key);  // hledání
htab_data_t *htab_lookup_add(htab_t * t, htab_key_t key);

bool htab_erase(htab_t * t, htab_key_t key);    // ruší zadaný záznam

// for_each: projde všechny záznamy a zavolá na ně funkci f
void htab_for_each(const htab_t * t, void (*f)(htab_data_t *data));

void htab_clear(htab_t * t);    // ruší všechny záznamy
void htab_free(htab_t * t);     // destruktor tabulky

htab_list_t* initList();
htab_list_item_t* createItem(htab_t*);
void insertFirst(htab_list_t*, htab_list_item_t*);

void removeFirst(htab_list_t*);
void freeList(htab_list_t*);

htab_data_t* listSearch(htab_list_t*, htab_key_t, bool);

void makeNewArg(htab_data_t*, htab_data_t*);
void makeNewRet(htab_data_t*, htab_data_t*);
htab_data_t* createData(htab_key_t, variableDatatype_t);

void copyDataFuncCall(htab_data_t*, htab_data_t*);


#endif // __HTAB_H__

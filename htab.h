#ifndef __HTAB_H__
#define __HTAB_H__

#include <string.h>     // size_t
#include <stdbool.h>    // bool


typedef struct htab_item htab_item_t;

typedef struct htab htab_t;

// Typy:
typedef const char * htab_key_t;        // typ klíče

// Tabulka:
struct htab
{
    int size;  /* actual count of records in table, will be (inc)/(dec)remented if we gonna add/delete element from table */
    size_t arr_size;    /* Constant number of pointers to elements (will be same with number in htab_init function) */
    htab_item_t* itemPtrsArr[];
};


// Dvojice dat v tabulce:
typedef struct htab_pair {
    htab_key_t    key;          //  name of variable/function
    char* type;
    char* datatype;
    int varIntVal;
    double varNumVal;
    char** funcParams;
    char** funcReturns;
} htab_pair_t;


struct htab_item
{
    htab_item_t* next;
    htab_pair_t *itemData;
};



// Rozptylovací (hash) funkce (stejná pro všechny tabulky v programu)
// Pokud si v programu definujete stejnou funkci, použije se ta vaše.
size_t htab_hash_function(htab_key_t str);

// Funkce pro práci s tabulkou:
htab_t *htab_init(size_t n);                    // konstruktor tabulky
htab_t *htab_move(size_t n, htab_t *from);      // přesun dat do nové tabulky
size_t htab_size(const htab_t * t);             // počet záznamů v tabulce
size_t htab_bucket_count(const htab_t * t);     // velikost pole

htab_pair_t *htab_find(htab_t * t, htab_key_t key);  // hledání
htab_pair_t *htab_lookup_add(htab_t * t, htab_key_t key);

bool htab_erase(htab_t * t, htab_key_t key);    // ruší zadaný záznam

// for_each: projde všechny záznamy a zavolá na ně funkci f
void htab_for_each(const htab_t * t, void (*f)(htab_pair_t *data));

void htab_clear(htab_t * t);    // ruší všechny záznamy
void htab_free(htab_t * t);     // destruktor tabulky

#endif // __HTAB_H__

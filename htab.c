#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
//#include <ctype.h>

#include "htab.h"
#include "error.h"

/* Function makes and returns hash from string according to standard you can see below */
size_t htab_hash_function(const char *str) {
    uint32_t h=0;     // must have 32 bits
    const unsigned char *p;
    for(p=(const unsigned char*)str; *p!='\0'; p++)
        h = 65599*h + *p;
    return h;
}



/* "Mother" of our hash-table. This function creates it !!! */
htab_t *htab_init(size_t n)
{
    htab_t *hashTable = malloc((sizeof(htab_item_t*) * n) + sizeof(htab_t));
    if(hashTable == NULL)
    {
        fprintf(stderr, "%s ---> ERROR: Hash-Table allocation failed", __func__ );  /* bruh... */
        errorExit(COMPILER_INTERN_ERR, 99);
    }
    hashTable->arr_size = n; /* Declared and CONSTANT count of array of pointers */
    hashTable->size = 0; /* But this one will change, according to count of actual recordings in table */
    for(size_t i = 0; i < n; i++)
    {
        hashTable->itemPtrsArr[i] = NULL;  /* STOP SEGFAULT */
    }

    return hashTable;
}



size_t htab_bucket_count(const htab_t * t)
{
    if (t == NULL) {
        fprintf(stderr, "%s ---> ERROR: Hash-table doesn't exist", __func__);
        errorExit(COMPILER_INTERN_ERR, 99);
    }

    return t->arr_size;
}



void htab_clear(htab_t *t)
{
    if (t == NULL)
    {
        fprintf(stderr, "%s ---> ERROR: No hash-table to be cleared. Ending program...", __func__);
        errorExit(COMPILER_INTERN_ERR, 99);
    }

    for(size_t i = 0; i < t->arr_size; i++)
    {
        htab_item_t *buffer = t->itemPtrsArr[i];
        while(buffer != NULL)
        {
            htab_item_t *save_next = buffer->next; /* Saves pointer to the next item */
            free((char*)buffer->itemData->key);
            free(buffer->itemData);
            free(buffer);

            buffer = save_next;                    /* buffer is now pointer to (saved above) next item */
        }
        t->itemPtrsArr[i] = NULL;
    }

    t->size = 0;  /* Size is actual count of records in table, SOOO now it's 0 */
}


/* Looking for element with key == "key" and returns pointer to it if was found.  *
 *  Otherwise, returns NULL                                                       */
htab_data_t * htab_find(htab_t * t, htab_key_t key)
{
    if(t == NULL)
    {
        fprintf(stderr, "%s ---> ERROR: Hash table doesn't exist", __func__ );
        errorExit(COMPILER_INTERN_ERR, 99);
    }

    /* If even hash of "key" wasn't found in table, there's no point in looking further */
    if(t->itemPtrsArr[htab_hash_function(key) % t->arr_size] == NULL)
    {
        return NULL;
    }

    /* If element (pair) with key == "key" will be found, this will be returned by function */
    htab_data_t * found;

    htab_item_t * buffer = t->itemPtrsArr[htab_hash_function(key) % t->arr_size];

    while(buffer != NULL)
    {
        if (strcmp(buffer->itemData->key, key) == 0) {
            found = buffer->itemData;
            return found;
        } else buffer = buffer->next;
    }

    /* ------------ This point will be reached if pair with the same key wasn't found -------------- */
    return NULL;
}


bool htab_erase(htab_t * t, htab_key_t key)
{
    if(t == NULL)
    {
        fprintf(stderr, "%s ---> ERROR: Hash-Table doesn't exist", __func__);
        errorExit(COMPILER_INTERN_ERR, 99);
    }

    if(htab_find(t, key) == NULL)  /* Checks, if item with key == "key" exists in table */
    {
        fprintf(stderr, "%s ---> ERROR: The node you want to delete wasn't found\n", __func__);
        errorExit(COMPILER_INTERN_ERR, 99);
    }

    htab_item_t * buffer = t->itemPtrsArr[(htab_hash_function(key) % t->arr_size)];

    while(buffer != NULL)
    {
        if (buffer->next == NULL)  /* If next element (according to buffer) points to nowhere(NULL) */
        {
            if(strcmp(buffer->itemData->key, key) == 0)
            {
                free((char*)buffer->itemData->key);
                free(buffer->itemData);
                free(buffer);
                t->itemPtrsArr[(htab_hash_function(key) % t->arr_size)] = NULL;
                t->size--;
                return true; /* Successfully deleted */
            }
        }
        else if (strcmp(buffer->next->itemData->key, key) == 0)  /* If next element points SOMEWHERE */
        {
            htab_item_t * tmp = buffer->next->next;
            free((char*)buffer->next->itemData->key);
            free(buffer->next->itemData);
            free(buffer->next);
            buffer->next = tmp;
            t->size--;
            return true;  /* Successfully deleted */
        }
        else buffer = buffer->next;
    }

    /* ------------- FUNCTION WILL NEVER REACH THIS POINT -------------- */
    return false;
}


void htab_for_each(const htab_t * t, void (*f)(htab_data_t *data))
{
    if (t == NULL)
    {
        fprintf(stderr, "%s ---> ERROR: Hash-table doesn't exist", __func__);
        errorExit(COMPILER_INTERN_ERR, 99);
    }

    /* Will constantly change in while-cycle */
    htab_item_t * buffer;

    /* Checking all pointers (nodes) in main linked-list (hash-table) */
    for(size_t i = 0; i < t->arr_size; i++)
    {
        buffer = t->itemPtrsArr[i];
        while (buffer != NULL)
        {
            (*f)(buffer->itemData);
            /* Also checking chain-elements */
            buffer = buffer->next;
        }
    }
}

void htab_free(htab_t *t)
{
    if (t == NULL)
    {
        fprintf(stderr, "%s ---> ERROR: No hash-table to be freed.", __func__);
        errorExit(COMPILER_INTERN_ERR, 99);
    }

    /* Clears whole table before... */
    htab_clear(t);

    /* KABOOOM */
    free(t);
}

/* Creates new hash-table, COPIES content of hash-table "from" to new hash-table
 *      then  CLEANS (not DELETES) "old" hash-table  */
htab_t *htab_move(size_t n, htab_t *from)
{
    if (from == NULL)
    {
        fprintf(stderr, "%s ---> ERROR: Hash-table that you want to move doesn't exist\n", __func__);
        errorExit(COMPILER_INTERN_ERR, 99);
    }

    htab_t* to = htab_init(n);  /* LOOK, a new hash-table is born. Nature is amazing... I'm crying... */

    if (to == NULL)
    {
        fprintf(stderr, "%s ---> ERROR: New Hash-Table wasn't initiated\n", __func__);
        errorExit(COMPILER_INTERN_ERR, 99);
    }



    for(size_t i = 0; i < from->arr_size; i++)
    {
        htab_item_t *buffer = from->itemPtrsArr[i];
        while (buffer != NULL)
        {
            htab_data_t *newPair = htab_lookup_add(to,buffer->itemData->key); /* buffer of new "pairs" key-value in new table */
            if(newPair == NULL)
            {
                fprintf(stderr, "%s ---> ERROR: Allocation of new pair failed\n", __func__);
                errorExit(COMPILER_INTERN_ERR, 99);
            }
            buffer = buffer->next;
        }
    }

    htab_clear(from);

    return to;

}

/* If element with key == "key" already exists - function returns pointer to it *
 *  Otherwise, creates NEW element in hash-table with key == "key"              */
htab_data_t * htab_lookup_add(htab_t * t, htab_key_t key)
{
    if(t == NULL)
    {
        errorExit(COMPILER_INTERN_ERR, 99);
    }

    /* If "key" already exists, returns pointer to pair
     *  where the"key" was found */
    //if(htab_find(t, key) != NULL) return htab_find(t, key);

    /* -------------- If "key" was found, here function run will stop ----------------- */

    /* Creating (so, allocating memory for) new element of hash-table */
    htab_item_t * new = malloc(sizeof(htab_item_t));
    new->itemData = malloc(sizeof(htab_data_t));
    new->itemData->key = malloc(sizeof(char) * (strlen(key) + 1));
    strcpy((char*) new->itemData->key, key);
    new->itemData->varIntVal = 0;
    new->itemData->varNumVal = 0;
    new->itemData->countOfArgs = 0;
    new->itemData->countOfReturns = 0;
    new->itemData->funcArgs = NULL;
    new->itemData->funcReturns = NULL;

    /* Points nowhere, because new element will be the last in "chain" */
    new->next = NULL;

    /* Checking if new item was successfully allocated */
    if(new == NULL || new->itemData == NULL || new->itemData->key == NULL)
    {
        fprintf(stderr, "%s ---> ERROR: Unable to create new item\n",__func__);
        errorExit(COMPILER_INTERN_ERR, 99);
    }



    if(t->itemPtrsArr[htab_hash_function(key) % t->arr_size] == NULL)
    {
        t->itemPtrsArr[htab_hash_function(key) % t->arr_size] = new;                /* If there's no chain in node with same hash - creates first (our new) element in this node */
        t->size++;     /* increments actual size of records in hash-table */
        return t->itemPtrsArr[htab_hash_function(key) % t->arr_size]->itemData;
    }

    /* Will constantly change in while-cycle */
    htab_item_t * buffer = t->itemPtrsArr[htab_hash_function(key) % t->arr_size];


    while(buffer != NULL)
    {
        if(strcmp(buffer->itemData->key, key) == 0)
        {
            return buffer->itemData;
        }
        if (buffer->next == NULL)
        {
            buffer->next = new;
            t->size++;
            return buffer->next->itemData;
        }
        buffer = buffer->next;
    }

    /* ---------- Will NEVER reach this point, made just for the function to return something ----------- */
    return NULL;
}

/* Returns actual size of your hash-table */
size_t htab_size(const htab_t * t)
{
    if (t == NULL)
    {
        fprintf(stderr, "%s ---> ERROR: Hash-table doesn't exist\n", __func__);
        errorExit(COMPILER_INTERN_ERR, 99);
    }

    return t->size;
}

htab_list_t* initList()
{
    htab_list_t* list = malloc(sizeof(htab_list_t));
    if(!list)
    {
        fprintf(stderr, "%s ERROR: can't allocate hashtable list! Exit...", __func__);
        errorExit(COMPILER_INTERN_ERR, 99);
    }

    list->first = NULL;

    return list;
}

htab_list_item_t* createItem(htab_t* toBePacked)
{
    if(!toBePacked)
    {
        fprintf(stderr, "%s ERROR: You're passing hashtable, that doesn't exist!!! Exit...\n", __func__);
        errorExit(COMPILER_INTERN_ERR, 99);
    }
    htab_list_item_t * newItem = malloc(sizeof(htab_list_item_t));

    if(!newItem)
    {
        fprintf(stderr, "%s ERROR: Can't allocate new item of list!!! Exit...\n", __func__);
        errorExit(COMPILER_INTERN_ERR, 99);
    }

    newItem->symtable = toBePacked;
    newItem->next = NULL;

    return newItem;
}

void insertFirst(htab_list_t* list, htab_list_item_t* item)
{
    if(!list || !item)
    {
        fprintf(stderr, "%s ERROR: can't create first element of hashtable list! Exit...", __func__ );
        errorExit(COMPILER_INTERN_ERR, 99);
    }

    if(!list->first)list->first = item;
    else
    {
        item->next = list->first;
        list->first = item;
    }

}

void removeFirst(htab_list_t* list)
{
    if(!list)
    {
        printf("%s ERROR: list doesn't exist!", __func__ );
        exit(228); // TODO
    }

    if(!list->first->next) free(list->first);
    else
    {
        htab_list_item_t* thanosSnap = list->first;
        list->first = list->first->next;
        free(thanosSnap);
    }
}

void freeList(htab_list_t* list)
{
    if(!list)
    {
        fprintf(stderr,"%s ERROR: list doesn't exist!", __func__ );
        errorExit(COMPILER_INTERN_ERR, 99);
    }

    while(list->first) removeFirst(list);

    free(list);
}

htab_data_t* listSearch(htab_list_t* list, htab_key_t key, bool choose)
{
    if(!list)
    {
        fprintf(stderr,"%s ERROR: list doesn't exist!", __func__ );
        errorExit(COMPILER_INTERN_ERR, 99);
    }

    htab_list_item_t* inspectorGadget = choose ? list->first->next : list->first;
    while(inspectorGadget)
    {
        if(htab_find(inspectorGadget->symtable, key)) return htab_find(inspectorGadget->symtable, key);
        inspectorGadget = inspectorGadget->next;
    }

    return NULL;
}

void makeNewArg(htab_data_t* functionData, htab_data_t* parameterData)
{
    functionData->countOfArgs++;
    functionData->funcArgs = realloc(functionData->funcArgs, sizeof(htab_data_t) * functionData->countOfArgs);
    functionData->funcArgs[functionData->countOfArgs - 1] = parameterData;
}

void makeNewRet(htab_data_t* functionData, htab_data_t* returnData)
{
    functionData->countOfReturns++;
    functionData->funcReturns = realloc(functionData->funcReturns, sizeof(htab_data_t) * functionData->countOfReturns);
    functionData->funcReturns[functionData->countOfReturns - 1] = returnData;
}

htab_data_t* createData(htab_key_t key, variableDatatype_t datatype)
{
    htab_data_t* newData = malloc(sizeof(htab_data_t));
    if(!newData)
    {
        printf("%s ERROR: Unable to create new data! Exit...", __func__);
        errorExit(COMPILER_INTERN_ERR, 99);
    }
    newData->key = key;
    newData->type = TYPE_VARIABLE; // default!!!
    newData->datatype = datatype;

    return newData;
}


void copyDataFuncCall(htab_data_t* from, htab_data_t* to)
{
    to->countOfArgs = from->countOfArgs;
    to->countOfReturns = from->countOfReturns;

    to->key = from->key;

    to->funcArgs = malloc(sizeof(htab_data_t) * to->countOfArgs);
    to->funcReturns = malloc(sizeof(htab_data_t) * to->countOfReturns);

    if(to->funcArgs || to->funcReturns)
    {
        fprintf(stderr, "ERROR in %s: Memory allocation failed\n", __func__);
        errorExit(COMPILER_INTERN_ERR, 99);
    }

    for(int i = 0; i < to->countOfArgs; i++)
    {
        htab_data_t * tmp = malloc(sizeof(htab_data_t));
        tmp->datatype = from->datatype;
        tmp->key = from->key;
        tmp->type = from->type;
        to->funcArgs[i] = tmp;
    }

    for(int i = 0; i < to->countOfReturns; i++)
    {
        htab_data_t * tmp = malloc(sizeof(htab_data_t));
        tmp->datatype = from->datatype;
        to->funcReturns[i] = tmp;
    }
}


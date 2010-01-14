/* Symbol table.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include <string.h>
#include "gc.h"

#include "table.h"
#include "object.h"
#include "error.h"


struct table_entry {
    struct table_entry *next;
    object *symbol;
};


#define HASH_SIZE 1021
static struct table_entry *symbol_table[HASH_SIZE];


static unsigned hash(char const *name)
{
    unsigned hashval;

    for (hashval = 0; *name != '\0'; name++) {
        hashval = (unsigned)*name + 31 * hashval;
    }
    return hashval % HASH_SIZE;
}


object *insert_symbol(object *symbol)
{
    if (symbol->type != SYMBOL) {
        error("not a symbol");
    }

    if (lookup_symbol(symbol->value.symbol) != NULL) {
        return symbol;
    }

    unsigned hashval = hash(symbol->value.symbol);
    struct table_entry *entry = GC_MALLOC(sizeof(struct table_entry));
    if (entry == NULL) {
        error("unable to allocate symbol table entry:");
    }

    entry->symbol = symbol;
    entry->next = symbol_table[hashval];
    symbol_table[hashval] = entry;

    return symbol;
}


object *lookup_symbol(char const *name)
{
    unsigned hashval = hash(name);
    struct table_entry *entry = symbol_table[hashval];

    while (entry != NULL) {
        if (strncmp(entry->symbol->value.symbol, name, strlen(name)) == 0) {
            return entry->symbol;
        }
        entry = entry->next;
    }
    return NULL;
}


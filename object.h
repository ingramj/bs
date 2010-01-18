/* Object model.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#ifndef OBJECT_H
#define OBJECT_H

#include "error.h"

typedef enum {
    NUMBER,
    BOOLEAN,
    CHARACTER,
    STRING,
    SYMBOL,
    EMPTY_LIST,
    PAIR,
    PRIMITIVE
} object_type;


typedef struct object {
    union {
        long number;
        int boolean;
        char character;
        char const *string;
        char const *symbol;
        struct {
            struct object *car;
            struct object *cdr;
        } pair;
        struct object *(*primitive)(struct object *arguments);
    } value;
    object_type type;
} object;


object *make_number(long value);
inline int is_number(object *obj) { return obj->type == NUMBER; }

object *get_boolean(int value);
int is_false(object *obj);
inline int is_boolean(object *obj) { return obj->type == BOOLEAN; }

object *make_character(char value);
inline int is_character(object *obj) { return obj->type == CHARACTER; }

object *make_string(char *value);
inline int is_string(object *obj) { return obj->type == STRING; }

object *make_symbol(char const *name);
inline int is_symbol(object *obj) { return obj->type == SYMBOL; }

object *get_empty_list(void);
inline int is_empty_list(object *obj) { return obj->type == EMPTY_LIST; }


object *cons(object *obj_car, object *obj_cdr);

inline int is_pair(object *obj) { return obj->type == PAIR; }

int is_list(object *obj);

inline object *car(object *pair)
{
    if (pair->type != PAIR) error("not a pair");
    return pair->value.pair.car;
}

inline void set_car(object *pair, object *obj)
{
    if (pair->type != PAIR) error("not a pair");
    pair->value.pair.car = obj;
}

inline object *cdr(object *pair)
{
    if (pair->type != PAIR) error("not a pair");
    return pair->value.pair.cdr;
}

inline void set_cdr(object *pair, object *obj)
{
    if (pair->type != PAIR) error("not a pair");
    pair->value.pair.cdr = obj;
}


object *make_primitive(object *(*fn)(object *args));
inline int is_primitive(object *obj) { return obj->type == PRIMITIVE; }

#endif


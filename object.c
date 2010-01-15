/* Object model.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include "gc.h"

#include "object.h"
#include "table.h"
#include "error.h"
#include "lexer.h"


static object *alloc_object(void);


static object empty_list = {.type = EMPTY_LIST };
static object true_object = {.type = BOOLEAN, .value.boolean = 1};
static object false_object = {.type = BOOLEAN, .value.boolean = 0};


static object *alloc_object(void)
{
    object *obj = GC_MALLOC(sizeof(object));
    if (obj == NULL) {
        error("unable to allocate an object:");
    }

    return obj;
}


object *make_number(long value)
{
    object *num = alloc_object();
    num->type = NUMBER;
    num->value.number = value;

    return num;
}


int is_number(object *obj)
{
    if (obj == NULL) {
        error("null object");
    }
    return obj->type == NUMBER;
}


object *get_boolean(int value)
{
    return value == 0 ? &false_object : &true_object;
}


int is_boolean(object *obj)
{
    if (obj == NULL) {
        error("null object");
    }
    return obj->type == BOOLEAN;
}


int is_false(object *obj)
{
    if (obj == NULL) {
        error("null object");
    }
    return (obj->type == BOOLEAN && obj->value.boolean == 0);
}


object *make_character(char value)
{
    object *c = alloc_object();
    c->type = CHARACTER;
    c->value.character = value;

    return c;
}


int is_character(object *obj)
{
    return obj->type == CHARACTER;
}


object *make_string(char *value)
{
    object *s = alloc_object();
    s->type = STRING;
    s->value.string = value;

    return s;
}


int is_string(object *obj)
{
    return obj->type == STRING;
}


object *get_empty_list(void)
{
    return &empty_list;
}


int is_empty_list(object *obj)
{
    return obj->type == EMPTY_LIST;
}


object *cons(object *obj_car, object *obj_cdr)
{
    object *p = alloc_object();
    p->type = PAIR;
    p->value.pair.car = obj_car;
    p->value.pair.cdr = obj_cdr;

    return p;
}


object *car(object *pair)
{
    if (pair->type != PAIR) {
        error("not a pair");
    }

    return pair->value.pair.car;
}


void set_car(object *pair, object *obj)
{
    if (pair->type != PAIR) {
        error("not a pair");
    }

    pair->value.pair.car = obj;
}


object *cdr(object *pair)
{
    if (pair->type != PAIR) {
        error("not a pair");
    }

    return pair->value.pair.cdr;
}


void set_cdr(object *pair, object *obj)
{
    if (pair->type != PAIR) {
        error("not a pair");
    }

    pair->value.pair.cdr = obj;
}


int is_pair(object *obj)
{
    return obj->type == PAIR;
}


object *make_symbol(char const *name)
{
    object *sym = lookup_symbol(name);
    if (sym == NULL) {
        sym = alloc_object();
        sym->type = SYMBOL;
        sym->value.symbol = name;
        return insert_symbol(sym);
    } else {
        return sym;
    }
}


int is_symbol(object *obj)
{
    return obj->type == SYMBOL;
}


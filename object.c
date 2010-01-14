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


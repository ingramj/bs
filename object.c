/* Object model.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include "gc.h"

#include "object.h"
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


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

extern int is_invalid(object *obj);
extern int is_number(object *obj);
extern int is_boolean(object *obj);
extern int is_character(object *obj);
extern int is_string(object *obj);
extern int is_symbol(object *obj);
extern int is_empty_list(object *obj);
extern int is_pair(object *obj);
extern object *car(object *pair);
extern void set_car(object *pair, object *obj);
extern object *cdr(object *pair);
extern void set_cdr(object *pair, object *obj);
extern int is_primitive_proc(object *obj);
extern int is_compound_proc(object *obj);
extern int is_procedure(object *obj);

static object *alloc_object(void);

static object invalid_object = { .type = INVALID };
static object empty_list = { .type = EMPTY_LIST };
static object true_object = { .type = BOOLEAN, .value.boolean = 1};
static object false_object = { .type = BOOLEAN, .value.boolean = 0};


static object *alloc_object(void)
{
    object *obj = GC_MALLOC(sizeof(object));
    if (obj == NULL) {
        error("unable to allocate an object:");
    }

    return obj;
}


object *get_invalid(void)
{
    return &invalid_object;
}


object *make_number(long value)
{
    object *num = alloc_object();
    num->type = NUMBER;
    num->value.number = value;

    return num;
}


object *get_boolean(int value)
{
    return value == 0 ? &false_object : &true_object;
}


int is_false(object *obj)
{
    return (obj->type == BOOLEAN && obj->value.boolean == 0);
}


object *make_character(char value)
{
    object *c = alloc_object();
    c->type = CHARACTER;
    c->value.character = value;

    return c;
}


object *make_string(char *value)
{
    object *s = alloc_object();
    s->type = STRING;
    s->value.string = value;

    return s;
}


object *get_empty_list(void)
{
    return &empty_list;
}


object *cons(object *obj_car, object *obj_cdr)
{
    object *p = alloc_object();
    p->type = PAIR;
    p->value.pair.car = obj_car;
    p->value.pair.cdr = obj_cdr;

    return p;
}


int is_list(object *obj)
{
    if (is_empty_list(obj)) {
        return 1;
    }

    while (is_pair(obj)) {
        if (is_empty_list(cdr(obj))) {
            return 1;
        }
        obj = cdr(obj);
    }

    return 0;
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


object *make_primitive_proc(object *(*fn)(object *args))
{
    object *prim = alloc_object();
    prim->type = PRIMITIVE_PROC;
    prim->value.primitive_proc = fn;

    return prim;
}


object *make_compound_proc(object *parameters, object *body, object *env)
{
    object *proc = alloc_object();
    proc->type = COMPOUND_PROC;
    proc->value.compound_proc.parameters = parameters;
    proc->value.compound_proc.body = body;
    proc->value.compound_proc.env = env;

    return proc;
}


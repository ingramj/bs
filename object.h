/* Object model.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#ifndef OBJECT_H
#define OBJECT_H

#include <stdio.h>

#include "error.h"

typedef enum {
    NUMBER,
    BOOLEAN,
    CHARACTER,
    STRING,
    SYMBOL,
    EMPTY_LIST,
    PAIR,
    PRIMITIVE_PROC,
    COMPOUND_PROC,
    END_OF_FILE,
    PORT
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
        struct object *(*primitive_proc)(struct object *arguments);
        struct {
            struct object *parameters;
            struct object *body;
            struct object *env;
        } compound_proc;
        struct {
            int mode;   // 0 for input, 1 for output
            int state;  // -1 for eof, 0 for closed, 1 for open
            FILE *file;
        } port;
    } value;
    object_type type;
} object;

object *get_end_of_file(void);
static inline int is_end_of_file(object *obj) { return obj->type == END_OF_FILE; }

object *make_number(long value);
static inline int is_number(object *obj) { return obj->type == NUMBER; }

object *get_boolean(int value);
int is_false(object *obj);
static inline int is_true(object *obj) { return !is_false(obj); }
static inline int is_boolean(object *obj) { return obj->type == BOOLEAN; }

object *make_character(char value);
static inline int is_character(object *obj) { return obj->type == CHARACTER; }

object *make_string(char *value);
static inline int is_string(object *obj) { return obj->type == STRING; }

object *make_symbol(char const *name);
static inline int is_symbol(object *obj) { return obj->type == SYMBOL; }

object *get_empty_list(void);
static inline int is_empty_list(object *obj) { return obj->type == EMPTY_LIST; }


object *cons(object *obj_car, object *obj_cdr);

static inline int is_pair(object *obj) { return obj->type == PAIR; }

int is_list(object *obj);

static inline object *car(object *pair)
{
    if (pair->type != PAIR) { error("not a pair"); }
    return pair->value.pair.car;
}

static inline void set_car(object *pair, object *obj)
{
    if (pair->type != PAIR) { error("cannot set car of non-pair"); }
    pair->value.pair.car = obj;
}

static inline object *cdr(object *pair)
{
    if (pair->type != PAIR) { error("not a pair"); }
    return pair->value.pair.cdr;
}

static inline void set_cdr(object *pair, object *obj)
{
    if (pair->type != PAIR) { error("cannot set cdr of non-pair"); }
    pair->value.pair.cdr = obj;
}


object *make_primitive_proc(object *(*fn)(object *args));
static inline int is_primitive_proc(object *obj)
{
    return obj->type == PRIMITIVE_PROC;
}

object *make_compound_proc(object *paramters, object *body, object *env);
static inline int is_compound_proc(object *obj) { return obj->type == COMPOUND_PROC; }

static inline int is_procedure(object *obj)
{
    return is_primitive_proc(obj) || is_compound_proc(obj);
}


object *make_input_port(char const *file);
object *make_output_port(char const *file);
static inline int is_port(object *obj) { return obj->type == PORT; }

static inline int is_input_port(object *obj)
{
    return is_port(obj) && obj->value.port.mode == 0;
}

static inline int is_output_port(object *obj)
{
    return is_port(obj) && obj->value.port.mode == 1;
}

#endif


/* Object model.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#ifndef OBJECT_H
#define OBJECT_H

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
int is_number(object *obj);

object *get_boolean(int value);
int is_boolean(object *obj);
int is_false(object *obj);

object *make_character(char value);
int is_character(object *obj);

object *make_string(char *value);
int is_string(object *obj);

object *make_symbol(char const *name);
int is_symbol(object *obj);

object *get_empty_list(void);
int is_empty_list(object *obj);

object *cons(object *obj_car, object *obj_cdr);
int is_pair(object *obj);
object *car(object *pair);
void set_car(object *pair, object *obj);
object *cdr(object *pair);
void set_cdr(object *pair, object *obj);

object *make_primitive(object *(*fn)(object *args));
int is_primitive(object *obj);

#endif


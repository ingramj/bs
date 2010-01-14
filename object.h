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
    EMPTY_LIST,
    PAIR
} object_type;


typedef struct object {
    union {
        long number;
        int boolean;
        char character;
        char *string;
        struct {
            struct object *car;
            struct object *cdr;
        } pair;
    } value;
    object_type type;
} object;


object *make_number(long value);
int is_number(object *obj);

int is_boolean(object *obj);
int is_false(object *obj);

object *make_character(char value);
int is_character(object *obj);

object *make_string(char *value);
int is_string(object *obj);

int is_empty_list(object *obj);

object *cons(object *obj_car, object *obj_cdr);
int is_pair(object *obj);
#endif


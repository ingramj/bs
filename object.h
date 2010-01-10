/* Object model.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#ifndef OBJECT_H
#define OBJECT_H

typedef enum {
    NUMBER,
    BOOLEAN
} object_type;


typedef struct object {
    union {
        long number;
        int boolean;
    } value;
    object_type type;
} object;


object *make_number(long value);
int is_number(object *obj);

int is_boolean(object *obj);
int is_false(object *obj);

#endif


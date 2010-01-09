/* Object model.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#ifndef OBJECT_H
#define OBJECT_H

typedef enum {
    NUMBER
} object_type;


typedef struct object {
    union {
        long number;
    } value;
    object_type type;
} object;


object *make_number(long value);
int is_number(object *obj);

#endif


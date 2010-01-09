/* Basic Scheme interpreter main loop.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include <stdio.h>
#include "gc.h"

#include "error.h"
#include "object.h"
#include "parser.h"


int main(void)
{
    printf("Parser test. Press ctrl-d to quit.\n");
    printf("bs> ");

    object *obj;
    while ((obj = bs_read())) {
        if (obj->type == NUMBER) {
            printf("%ld", obj->value.number);
        } else {
            error("unknown object type");
        }
        printf("\nbs> ");
    }

    printf("\n");
}


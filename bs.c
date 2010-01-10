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
#include "eval.h"
#include "print.h"


int main(void)
{
    printf("REPL test. Press ctrl-d to quit.\n");
    printf("bs> ");
    object *obj = bs_read(stdin);
    while (obj) {
        bs_write(stdout, bs_eval(obj));
        printf("\nbs> ");
        obj = bs_read(stdin);
    }

    printf("\n");
}


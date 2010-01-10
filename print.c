/* Expression printer for a Scheme-like language.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include <stdio.h>

#include "print.h"
#include "object.h"
#include "error.h"


void bs_write(FILE *out, object *exp)
{
    if (!exp) {
        error("unable to write expression");
    }

    switch (exp->type) {
        case NUMBER:
            fprintf(out, "%ld", exp->value.number);
            break;
        case BOOLEAN:
            fprintf(out, "#%c", (exp->value.boolean == 0 ? 'f' : 't'));
            break;
        default:
            error("unknown expression type.");
    }
}


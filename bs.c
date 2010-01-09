/* Basic Scheme interpreter main loop.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include <stdio.h>
#include "gc.h"

#include "error.h"
#include "lexer.h"

static void print_token(token const * const t)
{
    switch (t->type) {
        case NUMBER:
            printf("NUM(%ld) ", t->value.number);
            break;
        default:
            error("unknown token type.");
    }
}

int main(void)
{
    printf("Lexer test. Press ctrl-d to quit.\n");
    printf("bs> ");

    token *t = get_token();
    while (t->type != DONE) {

        while (t->type != EOL) {
            print_token(t);
            printf("\n");
            t = get_token();
        }

        printf("bs> ");
        t = get_token();
    }

    printf("\n");
}


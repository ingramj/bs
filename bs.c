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
#include "lexer.h"
#include "eval.h"
#include "print.h"


int main(int argc, char *argv[])
{
    GC_INIT();

    FILE *in = stdin;
    if (argc == 2) {
        in = fopen(argv[1], "r");
        if (in == NULL) {
            error("could not read file %s:", argv[1]);
        }
    } else if (argc > 2) {
        printf("usage: bs [infile]\n");
        exit(1);
    }

    set_input_file(in);

    if(in == stdin) {
        printf("REPL test. Press ctrl-d to quit.\n");
        printf("bs> ");
    }
    object *obj = bs_read();
    while (obj) {
        bs_write(stdout, bs_eval(obj));
        printf("\n");
        if (in == stdin) printf("bs> ");
        obj = bs_read();
    }

    printf("\n");
}


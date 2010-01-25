/* Basic Scheme interpreter main loop.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include <stdio.h>
#include "gc.h"

#include "environment.h"
#include "error.h"
#include "eval.h"
#include "lexer.h"
#include "object.h"
#include "port.h"
#include "primitive.h"
#include "table.h"
#include "read.h"
#include "write.h"


void init_system(void)
{
    GC_INIT();
    set_error_level(WARNING);
    init_special_forms();
    init_global_environment();
    init_primitives(get_global_environment());
    init_standard_ports();
}


int main(int argc, char *argv[])
{
    init_system();

    object *input_port;
    if (argc == 2) {
        input_port = make_input_port(argv[1]);
    } else if (argc > 2) {
        write_output("usage: bs [infile]\n");
        exit(1);
    } else {
        input_port = get_standard_input_port();
    }

    set_input_port(input_port);

    if(input_port == get_standard_input_port()) {
        write_output("Welcome to the bs REPL. Press ctrl-d to quit.");
        write_output("\nbs> ");
    }

    object *obj = bs_read();
    while (!is_end_of_file(obj)) {
        bs_write(bs_eval(obj, get_global_environment()));
        write_output("\n");
        if (input_port == get_standard_input_port()) {
            write_output("bs> ");
        }
        obj = bs_read();
    }

    write_output("\n");

    return 0;
}


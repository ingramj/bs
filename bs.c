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
#include "file.h"
#include "eval.h"
#include "environment.h"
#include "primitive.h"
#include "print.h"
#include "table.h"


void init_system(void)
{
    GC_INIT();
    set_error_level(WARNING);
    init_special_forms();
    init_environments();
    init_primitives();
    init_standard_ports();
}


int main(int argc, char *argv[])
{
    init_system();

    object *input_port;
    if (argc == 2) {
        input_port = make_input_port(argv[1]);
    } else if (argc > 2) {
        printf("usage: bs [infile]\n");
        exit(1);
    } else {
        input_port = get_standard_input_port();
    }

    set_current_input_port(input_port);

    if(input_port == get_standard_input_port()) {
        write_line(get_current_output_port(),
                "Welcome to the bs REPL. Press ctrl-d to quit.\n");
        write_line(get_current_output_port(), "bs> ");
    }

    object *obj = bs_read();
    while (!is_end_of_file(obj)) {
        if (!is_invalid(obj)) {
            bs_write(get_current_output_port(),
                    bs_eval(obj, get_global_environment()));
        }
        write_line(get_current_output_port(), "\n");
        if (input_port == get_standard_input_port()) {
            write_line(get_current_output_port(), "bs> ");
        }
        obj = bs_read();
    }

    write_line(get_current_output_port(), "\n");

    return 0;
}


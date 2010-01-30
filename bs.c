/* Basic Scheme interpreter main loop.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include <stdio.h>
#include <string.h>
#include "gc.h"

#include "environment.h"
#include "error.h"
#include "eval.h"
#include "object.h"
#include "port.h"
#include "primitive.h"
#include "read.h"
#include "write.h"


struct config {
    int print_results;
    object *input_port;
};

void init_system(void);
void print_usage(void);
struct config *parse_options(int argc, char *argv[]);


int main(int argc, char *argv[])
{
    init_system();

    struct config *conf = parse_options(argc, argv);
    set_input_port(conf->input_port);

    object *obj = bs_read();
    while (!is_end_of_file(obj)) {
        object *result = bs_eval(obj, get_global_environment());
        if (conf->print_results) {
            bs_write(result);
            write_output("\n");
        }
        obj = bs_read();
    }
    return 0;
}


void init_system(void)
{
    GC_INIT();
    init_standard_ports();
    set_error_level(WARNING);
    init_special_forms();
    init_global_environment();
    init_primitives(get_global_environment());
}


void print_usage(void)
{
    write_error("usage: bs file [-p]\n");
    write_error("file : a scheme source file, or '-' to read from stdin.\n");
    write_error("-p   : print the result of each expression in file.\n");
}


struct config *parse_options(int argc, char *argv[])
{
    if (argc < 2 || argc > 3) {
        print_usage();
        exit(1);
    }

    struct config *conf = GC_MALLOC(sizeof(struct config));
    if (conf == NULL) {
        error("could not allocate config struct");
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0) {
            conf->print_results = 1;
        } else if (argv[i][0] == '-' && argv[i][1] != '\0') {
            print_usage();
            exit(1);
        } else if (conf->input_port == NULL) {
            if (strcmp(argv[i], "-") == 0) {
                conf->input_port = get_standard_input_port();
            } else {
                conf->input_port = make_input_port(argv[i]);
            }
        } else {
            print_usage();
            exit(1);
        }
    }


    if (conf->input_port == NULL) {
        print_usage();
        exit(1);
    }

    return conf;
}


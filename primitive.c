/* Primitive procedure definitions.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include "primitive.h"
#include "error.h"
#include "object.h"
#include "eval.h"


object *add_proc(object *arguments)
{
    long result = 0;

    while (!is_empty_list(arguments)) {
        result += (car(arguments))->value.number;
        arguments = cdr(arguments);
    }
    return make_number(result);
}


object *sub_proc(object *arguments)
{
    if (is_empty_list(arguments)) {
        error("- requires at least one argument");
    }

    long result = car(arguments)->value.number;
    arguments = cdr(arguments);
    if (is_empty_list(arguments)) {
        return make_number(-result);
    }

    while (!is_empty_list(arguments)) {
        result -= (car(arguments))->value.number;
        arguments = cdr(arguments);
    }
    return make_number(result);
}


void init_primitives(void)
{
    define_variable(make_symbol("+"),
            make_primitive(add_proc),
            get_global_environment());

    define_variable(make_symbol("-"),
            make_primitive(sub_proc),
            get_global_environment());
}


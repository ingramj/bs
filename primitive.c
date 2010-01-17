/* Primitive procedure definitions.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include "primitive.h"
#include "error.h"
#include "object.h"
#include "eval.h"

#define defprim(name, proc) \
    define_variable(make_symbol(name), \
            make_primitive(proc), \
            get_global_environment())

static object *add_proc(object *arguments)
{
    long result = 0;

    while (!is_empty_list(arguments)) {
        if (!is_number(car(arguments))) {
            error("+ called with non-numeric arguments");
        }
        result += car(arguments)->value.number;
        arguments = cdr(arguments);
    }
    return make_number(result);
}


static object *sub_proc(object *arguments)
{
    if (is_empty_list(arguments)) {
        error("- requires at least one argument");
    }

    if (!is_number(car(arguments))) {
        error("- called with non-numeric arguments");
    }

    long result = car(arguments)->value.number;
    arguments = cdr(arguments);
    if (is_empty_list(arguments)) {
        return make_number(-result);
    }

    while (!is_empty_list(arguments)) {
        if (!is_number(car(arguments))) {
            error("- called with non-numeric arguments");
        }
        result -= (car(arguments))->value.number;
        arguments = cdr(arguments);
    }
    return make_number(result);
}


static object *mult_proc(object *arguments)
{
    long result = 1;

    while (!is_empty_list(arguments)) {
        if (!is_number(car(arguments))) {
            error("* called with non-numeric arguments");
        }
        result *= (car(arguments))->value.number;
        arguments = cdr(arguments);
    }
    return make_number(result);
}


static object *num_eq_proc(object *arguments)
{
    if (is_empty_list(arguments) || is_empty_list(cdr(arguments))) {
        error("= requires at least two arguments");
    }

    while (!is_empty_list(arguments) && ! is_empty_list(cdr(arguments))) {
        if (!is_number(car(arguments)) || !is_number(car(cdr(arguments)))) {
            error("= called with non-numeric arguments");
        }
        long n1 = car(arguments)->value.number;
        long n2 = car(cdr(arguments))->value.number;
        if ((n1 - n2) != 0) {
            return get_boolean(0);
        }
        arguments = cdr(arguments);
    }
    return get_boolean(1);
}

void init_primitives(void)
{
    defprim("+", add_proc);
    defprim("-", sub_proc);
    defprim("*", mult_proc);
    defprim("=", num_eq_proc);
}


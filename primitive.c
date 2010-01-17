/* Primitive procedure definitions.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include <string.h>

#include "primitive.h"
#include "error.h"
#include "object.h"
#include "eval.h"

#define defprim(name, proc) \
    define_variable(make_symbol(name), \
            make_primitive(proc), \
            get_global_environment())


static object *eq_proc(object *arguments)
{
    if (is_empty_list(arguments) || !is_empty_list(cdr(cdr(arguments)))) {
        error("eq? requires exactly two arguments");
    }

    object *o1 = car(arguments);
    object *o2 = car(cdr(arguments));

    if (o1->type != o2->type) {
        return get_boolean(0);
    }

    switch (o1->type) {
        case NUMBER:
            return get_boolean(o1->value.number == o2->value.number);
        case CHARACTER:
            return get_boolean(o1->value.character == o2->value.character);
        case STRING:
            return get_boolean(
                    strcmp(o1->value.string, o2->value.string) == 0);
        default:
            return get_boolean(o1 == o2);
    }
}


static object *is_null_proc(object *arguments)
{
    if (is_empty_list(arguments) || !is_empty_list(cdr(arguments))) {
        error("null? requires a single argument");
    }

    return get_boolean(is_empty_list(car(arguments)));
}


static object *is_boolean_proc(object *arguments)
{
    if (is_empty_list(arguments) || !is_empty_list(cdr(arguments))) {
        error("boolean? requires a single argument");
    }

    return get_boolean(is_boolean(car(arguments)));
}


static object *is_symbol_proc(object *arguments)
{
    if (is_empty_list(arguments) || !is_empty_list(cdr(arguments))) {
        error("symbol? requires a single argument");
    }

    return get_boolean(is_symbol(car(arguments)));
}


static object *is_integer_proc(object *arguments)
{
    if (is_empty_list(arguments) || !is_empty_list(cdr(arguments))) {
        error("integer? requires a single argument");
    }

    return get_boolean(is_number(car(arguments)));
}


static object *is_char_proc(object *arguments)
{
    if (is_empty_list(arguments) || !is_empty_list(cdr(arguments))) {
        error("char? requires a single argument");
    }

    return get_boolean(is_character(car(arguments)));
}


static object *is_string_proc(object *arguments)
{
    if (is_empty_list(arguments) || !is_empty_list(cdr(arguments))) {
        error("string? requires a single argument");
    }

    return get_boolean(is_string(car(arguments)));
}


static object *is_pair_proc(object *arguments)
{
    if (is_empty_list(arguments) || !is_empty_list(cdr(arguments))) {
        error("pair? requires a single argument");
    }

    return get_boolean(is_pair(car(arguments)));
}


static object *is_procedure_proc(object *arguments)
{
    if (is_empty_list(arguments) || !is_empty_list(cdr(arguments))) {
        error("procedure? requires a single argument");
    }

    return get_boolean(is_primitive(car(arguments)));
}


static object *length_proc(object *arguments)
{
    if (is_empty_list(arguments) || !is_empty_list(cdr(arguments))) {
        error("length requires a single list as an argument");
    }

    long result = 0;
    arguments = car(arguments);
    while (!is_empty_list(arguments)) {
        result++;
        arguments = cdr(arguments);
    }

    return make_number(result);
}


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
    defprim("eq?", eq_proc);
    defprim("null?", is_null_proc);
    defprim("boolean?", is_boolean_proc);
    defprim("symbol?", is_symbol_proc);
    defprim("integer?", is_integer_proc);
    defprim("char?", is_char_proc);
    defprim("string?", is_string_proc);
    defprim("pair?", is_pair_proc);
    defprim("procedure?", is_procedure_proc);
    defprim("length", length_proc);
    defprim("+", add_proc);
    defprim("-", sub_proc);
    defprim("*", mult_proc);
    defprim("=", num_eq_proc);
}


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
#include "table.h"

#define defprim(name, proc) \
    define_variable(make_symbol(name), \
            make_primitive(proc), \
            get_global_environment())


#define require_exactly_one(args, name) \
    if (is_empty_list(args) || !is_empty_list(cdr(arguments))) { \
        error(name " requires a single argument"); \
    }


#define require_exactly_two(args, name) \
    if (is_empty_list(args) || is_empty_list(cdr(args)) || \
            !is_empty_list(cdr(cdr(arguments)))) { \
        error(name " requires two arguments"); \
    }


#define require_at_least_one(args, name) \
    if (is_empty_list(args)) { error(name " requires at least one argument"); }


#define require_at_least_two(args, name) \
    if (is_empty_list(args) || is_empty_list(cdr(args))) { \
        error(name " requires at least two arguments"); \
    }


#define require_number(arg, name) \
    if (!is_number(arg)) { error(name " called with non-numeric argument"); }


#define require_pair(arg, name) \
    if (!is_pair(arg)) { error(name " called with non-pair argument"); }


static object *eq_proc(object *arguments)
{
    require_exactly_two(arguments, "eq?");

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
    require_exactly_one(arguments, "null?");
    return get_boolean(is_empty_list(car(arguments)));
}


static object *is_boolean_proc(object *arguments)
{
    require_exactly_one(arguments, "boolean?");
    return get_boolean(is_boolean(car(arguments)));
}


static object *is_symbol_proc(object *arguments)
{
    require_exactly_one(arguments, "symbol?");
    return get_boolean(is_symbol(car(arguments)));
}


static object *is_integer_proc(object *arguments)
{
    require_exactly_one(arguments, "integer?");
    return get_boolean(is_number(car(arguments)));
}


static object *is_char_proc(object *arguments)
{
    require_exactly_one(arguments, "char?");
    return get_boolean(is_character(car(arguments)));
}


static object *is_string_proc(object *arguments)
{
    require_exactly_one(arguments, "string?");
    return get_boolean(is_string(car(arguments)));
}


static object *is_pair_proc(object *arguments)
{
    require_exactly_one(arguments, "pair?");
    return get_boolean(is_pair(car(arguments)));
}


static object *is_list_proc(object *arguments)
{
    require_exactly_one(arguments, "list?");
    return get_boolean(is_list(car(arguments)));
}


static object *is_procedure_proc(object *arguments)
{
    require_exactly_one(arguments, "procedure?");
    return get_boolean(is_primitive(car(arguments)));
}


static object *add_proc(object *arguments)
{
    long result = 0;

    while (!is_empty_list(arguments)) {
        require_number(car(arguments), "+");
        result += car(arguments)->value.number;
        arguments = cdr(arguments);
    }
    return make_number(result);
}


static object *sub_proc(object *arguments)
{
    require_at_least_one(arguments, "-");
    require_number(car(arguments), "-");

    long result = car(arguments)->value.number;
    arguments = cdr(arguments);
    if (is_empty_list(arguments)) {
        return make_number(-result);
    }

    while (!is_empty_list(arguments)) {
        require_number(car(arguments), "-");
        result -= (car(arguments))->value.number;
        arguments = cdr(arguments);
    }
    return make_number(result);
}


static object *mult_proc(object *arguments)
{
    long result = 1;

    while (!is_empty_list(arguments)) {
        require_number(car(arguments), "*");
        result *= (car(arguments))->value.number;
        arguments = cdr(arguments);
    }
    return make_number(result);
}


static object *num_eq_proc(object *arguments)
{
    require_at_least_two(arguments, "=");

    while (!is_empty_list(arguments) && ! is_empty_list(cdr(arguments))) {
        require_number(car(arguments), "=");
        require_number(car(cdr(arguments)), "=");
        long n1 = car(arguments)->value.number;
        long n2 = car(cdr(arguments))->value.number;
        if ((n1 - n2) != 0) {
            return get_boolean(0);
        }
        arguments = cdr(arguments);
    }
    return get_boolean(1);
}


static object *cons_proc(object *arguments)
{
    require_exactly_two(arguments, "cons");
    return cons(car(arguments), car(cdr(arguments)));
}


static object *car_proc(object *arguments)
{
    require_exactly_one(arguments, "car");
    require_pair(car(arguments), "car");
    return car(car(arguments));
}


static object *set_car_proc(object *arguments)
{
    require_exactly_two(arguments, "set-car!");
    require_pair(car(arguments), "set-car!");
    set_car(car(arguments), car(cdr(arguments)));
    return lookup_symbol("ok");
}


static object *cdr_proc(object *arguments)
{
    require_exactly_one(arguments, "cdr");
    require_pair(car(arguments), "cdr");
    return cdr(car(arguments));
}


static object *set_cdr_proc(object *arguments)
{
    require_exactly_two(arguments, "set-cdr!");
    require_pair(car(arguments), "set-cdr!");
    set_cdr(car(arguments), car(cdr(arguments)));
    return lookup_symbol("ok");
}


static object *length_proc(object *arguments)
{
    require_exactly_one(arguments, "length");

    arguments = car(arguments);

    if (is_empty_list(arguments)) {
        return make_number(0);
    }

    long result = 0;
    while (is_pair(arguments)) {
        result++;
        if (is_empty_list(cdr(arguments))) {
            return make_number(result);
        }
        arguments = cdr(arguments);
    }

    error("length requires a proper list as an argument");
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
    defprim("list?", is_list_proc);
    defprim("procedure?", is_procedure_proc);
    defprim("+", add_proc);
    defprim("-", sub_proc);
    defprim("*", mult_proc);
    defprim("=", num_eq_proc);
    defprim("cons", cons_proc);
    defprim("car", car_proc);
    defprim("set-car!", set_car_proc);
    defprim("cdr", cdr_proc);
    defprim("set-cdr!", set_cdr_proc);
    defprim("length", length_proc);
}


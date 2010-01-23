/* Primitive procedure definitions.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include "gc.h"

#include "environment.h"
#include "error.h"
#include "eval.h"
#include "object.h"
#include "port.h"
#include "primitive.h"
#include "read.h"
#include "table.h"

#define defprim(name, proc) \
    define_variable(make_symbol(name), \
            make_primitive_proc(proc), \
            get_global_environment())


#define require_exactly_one(args, name) \
    if (is_empty_list(args) || !is_empty_list(cdr(arguments))) { \
        warn(name " requires a single argument"); \
        return get_invalid(); \
    }


#define require_exactly_two(args, name) \
    if (is_empty_list(args) || is_empty_list(cdr(args)) || \
            !is_empty_list(cdr(cdr(arguments)))) { \
        warn(name " requires two arguments"); \
        return get_invalid(); \
    }


#define require_at_least_one(args, name) \
    if (is_empty_list(args)) { \
        warn(name " requires at least one argument"); \
        return get_invalid(); \
    }


#define require_at_least_two(args, name) \
    if (is_empty_list(args) || is_empty_list(cdr(args))) { \
        warn(name " requires at least two arguments"); \
        return get_invalid(); \
    }


#define require_number(arg, name) \
    if (!is_number(arg)) { \
        warn(name " called with non-numeric argument"); \
        return get_invalid(); \
    }


#define require_string(arg, name) \
    if (!is_string(arg)) { \
        warn(name " called with non-string argument"); \
        return get_invalid(); \
    }


#define require_symbol(arg, name) \
    if (!is_symbol(arg)) { \
        warn(name " called with non-symbol argument"); \
        return get_invalid(); \
    }


#define require_pair(arg, name) \
    if (!is_pair(arg)) { \
        warn(name " called with non-pair argument"); \
        return get_invalid(); \
    }


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
    return get_boolean(is_procedure(car(arguments)));
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


static object *quotient_proc(object *arguments)
{
    require_exactly_two(arguments, "quotient");
    object *n1 = car(arguments);
    object *n2 = car(cdr(arguments));
    require_number(n1, "quotient");
    require_number(n2, "quotient");
    if (n2->value.number == 0) {
        warn("divide by zero");
        return get_invalid();
    }
    ldiv_t d = ldiv(n1->value.number, n2->value.number);
    return make_number(d.quot);
}


static object *remainder_proc(object *arguments)
{
    require_exactly_two(arguments, "remainder");
    object *n1 = car(arguments);
    object *n2 = car(cdr(arguments));
    require_number(n1, "remainder");
    require_number(n2, "remainder");
    if (n2->value.number == 0) {
        warn("divide by zero");
        return get_invalid();
    }
    ldiv_t d = ldiv(n1->value.number, n2->value.number);
    return make_number(d.rem);
}


static object *num_eq_proc(object *arguments)
{
    require_at_least_two(arguments, "=");

    while (!is_empty_list(arguments) && !is_empty_list(cdr(arguments))) {
        object *n1 = car(arguments);
        object *n2 = car(cdr(arguments));
        require_number(n1, "=");
        require_number(n2, "=");
        if (n1->value.number != n2->value.number) {
            return get_boolean(0);
        }
        arguments = cdr(arguments);
    }
    return get_boolean(1);
}


static object *num_lt_proc(object *arguments)
{
    require_at_least_two(arguments, "<");

    while (!is_empty_list(arguments) && !is_empty_list(cdr(arguments))) {
        object *n1 = car(arguments);
        object *n2 = car(cdr(arguments));
        require_number(n1, "<");
        require_number(n2, "<");
        if (n1->value.number >= n2->value.number) {
            return get_boolean(0);
        }
        arguments = cdr(arguments);
    }
    return get_boolean(1);
}


static object *num_gt_proc(object *arguments)
{
    require_at_least_two(arguments, ">");

    while (!is_empty_list(arguments) && !is_empty_list(cdr(arguments))) {
        object *n1 = car(arguments);
        object *n2 = car(cdr(arguments));
        require_number(n1, ">");
        require_number(n2, ">");
        if (n1->value.number <= n2->value.number) {
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

    warn("length requires a proper list as an argument");
    return get_invalid();
}


static object *list_proc(object *arguments)
{
    return arguments;
}


static object *char_to_integer_proc(object *arguments)
{
    require_exactly_one(arguments, "char->integer");
    if (!is_character(car(arguments))) {
        warn("char->integer requires a character as an argument");
        return get_invalid();
    }
    char value = car(arguments)->value.character;
    return make_number(value);
}


static object *integer_to_char_proc(object *arguments)
{
    require_exactly_one(arguments, "integer->char");
    require_number(car(arguments), "integer->char");

    long value = car(arguments)->value.number;
    if (value > CHAR_MAX || value < CHAR_MIN) {
        warn("integer out of range for conversion to char");
        return get_invalid();
    }

    return make_character((char) value);
}


static object *number_to_string_proc(object *arguments)
{
    require_exactly_one(arguments, "number->string");
    require_number(car(arguments), "number->string");

    long value = car(arguments)->value.number;
    long num = value;
    unsigned digits = 0;
    if (num <= 0) {
        digits += 1;
        num *= -1;
    }

    while (num) {
        num /= 10;
        digits++;
    }

    char *buffer = GC_MALLOC(digits + 1);
    if (buffer == NULL) {
        error("unable to allocate string buffer:");
    }
    snprintf(buffer, digits + 1, "%ld", value);

    return make_string(buffer);
}


static object *string_to_number_proc(object *arguments)
{
    require_exactly_one(arguments, "string->number");
    require_string(car(arguments), "string->number");

    char const *s = car(arguments)->value.string;
    char *end;
    errno = 0;
    long num = strtol(s, &end, 0);

    if (end != (s + strlen(s))) {
        warn("string->number argument does not look like a number");
        return get_invalid();
    } else if (errno) {
        warn("unable to convert string to number:");
        return get_invalid();
    }

    return make_number(num);
}


static object *symbol_to_string_proc(object *arguments)
{
    require_exactly_one(arguments, "symbol->string");
    require_symbol(car(arguments), "symbol->string");

    char const *sym = car(arguments)->value.symbol;
    char *str = GC_MALLOC(strlen(sym) + 1);
    if (str == NULL) {
        error("unable to allocate string buffer:");
    }
    strcpy(str, sym);
    return make_string(str);
}


static object *string_to_symbol_proc(object *arguments)
{
    /* This procedure can create a symbol that contains invalid characters. */
    require_exactly_one(arguments, "string->symbol");
    require_string(car(arguments), "string->symbol");

    char const *str = car(arguments)->value.string;
    char *sym = GC_MALLOC(strlen(str) + 1);
    if (sym == NULL) {
        error("unable to allocate symbol buffer:");
    }
    strcpy(sym, str);
    return make_symbol(sym);
}


static object *load_proc(object *arguments)
{
    require_exactly_one(arguments, "load");
    require_string(car(arguments), "load");

    char const *src_file = car(arguments)->value.string;
    object *input_port = make_input_port(src_file);

    object *prev_port = get_current_input_port();
    set_current_input_port(input_port);

    object *obj = bs_read();
    while (!is_end_of_file(obj)) {
        bs_eval(obj, get_global_environment());
        obj = bs_read();
    }
    set_current_input_port(prev_port);
    close_port(input_port);

    return lookup_symbol("ok");
}


object *apply_proc(object *arguments)
{
    (void)arguments;    // unused argument.
    error("something's real bad wrong if this function was called");
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
    defprim("quotient", quotient_proc);
    defprim("remainder", remainder_proc);
    defprim("=", num_eq_proc);
    defprim("<", num_lt_proc);
    defprim(">", num_gt_proc);
    defprim("cons", cons_proc);
    defprim("car", car_proc);
    defprim("set-car!", set_car_proc);
    defprim("cdr", cdr_proc);
    defprim("set-cdr!", set_cdr_proc);
    defprim("length", length_proc);
    defprim("list", list_proc);
    defprim("char->integer", char_to_integer_proc);
    defprim("integer->char", integer_to_char_proc);
    defprim("number->string", number_to_string_proc);
    defprim("string->number", string_to_number_proc);
    defprim("symbol->string", symbol_to_string_proc);
    defprim("string->symbol", string_to_symbol_proc);
    defprim("load", load_proc);
    defprim("apply", apply_proc);
}


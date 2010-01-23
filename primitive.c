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

#define defproc(name, proc) \
    define_variable(make_symbol(name), \
            make_primitive_proc(proc), \
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
    if (is_empty_list(args)) { \
        error(name " requires at least one argument"); \
    }


#define require_at_least_two(args, name) \
    if (is_empty_list(args) || is_empty_list(cdr(args))) { \
        error(name " requires at least two arguments"); \
    }


#define require_number(arg, name) \
    if (!is_number(arg)) { \
        error(name " called with non-numeric argument"); \
    }


#define require_string(arg, name) \
    if (!is_string(arg)) { \
        error(name " called with non-string argument"); \
    }


#define require_symbol(arg, name) \
    if (!is_symbol(arg)) { \
        error(name " called with non-symbol argument"); \
    }


#define require_pair(arg, name) \
    if (!is_pair(arg)) { \
        error(name " called with non-pair argument"); \
    }


/**** Equality and type predicates ****/
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


/**** Arithmetic ****/
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
        error("divide by zero");
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
        error("divide by zero");
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


/**** List manipulation ****/
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


static object *list_proc(object *arguments)
{
    return arguments;
}


/**** Type conversion ****/
static object *char_to_integer_proc(object *arguments)
{
    require_exactly_one(arguments, "char->integer");
    if (!is_character(car(arguments))) {
        error("char->integer requires a character as an argument");
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
        error("integer out of range for conversion to char");
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
        error("string->number argument does not look like a number");
    } else if (errno) {
        error("unable to convert string to number:");
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


/**** Input/Output ****/
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


/**** Eval/Apply ****/
object *apply_proc(object *arguments)
{
    (void)arguments;    // unused argument.
    error("something's real bad wrong if this function was called");
}


void init_primitives(void)
{
    defproc("eq?", eq_proc);
    defproc("null?", is_null_proc);
    defproc("boolean?", is_boolean_proc);
    defproc("symbol?", is_symbol_proc);
    defproc("integer?", is_integer_proc);
    defproc("char?", is_char_proc);
    defproc("string?", is_string_proc);
    defproc("pair?", is_pair_proc);
    defproc("list?", is_list_proc);
    defproc("procedure?", is_procedure_proc);
    defproc("+", add_proc);
    defproc("-", sub_proc);
    defproc("*", mult_proc);
    defproc("quotient", quotient_proc);
    defproc("remainder", remainder_proc);
    defproc("=", num_eq_proc);
    defproc("<", num_lt_proc);
    defproc(">", num_gt_proc);
    defproc("cons", cons_proc);
    defproc("car", car_proc);
    defproc("set-car!", set_car_proc);
    defproc("cdr", cdr_proc);
    defproc("set-cdr!", set_cdr_proc);
    defproc("length", length_proc);
    defproc("list", list_proc);
    defproc("char->integer", char_to_integer_proc);
    defproc("integer->char", integer_to_char_proc);
    defproc("number->string", number_to_string_proc);
    defproc("string->number", string_to_number_proc);
    defproc("symbol->string", symbol_to_string_proc);
    defproc("string->symbol", string_to_symbol_proc);
    defproc("load", load_proc);
    defproc("apply", apply_proc);
}


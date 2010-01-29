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
#include "write.h"

#define defproc(name, proc, env) \
    define_variable(make_symbol(name), \
            make_primitive_proc(proc), \
            env)


#define require_zero(args, name) \
    if (!is_empty_list(args)) { \
        error(name " takes no arguments"); \
    }


#define require_at_most_one(args, name) \
    if (!is_empty_list(args) && !is_empty_list(cdr(args))) { \
        error(name " takes at most one argument"); \
    }


#define require_exactly_one(args, name) \
    if (is_empty_list(args) || !is_empty_list(cdr(args))) { \
        error(name " requires a single argument"); \
    }


#define require_one_or_two(args, name) \
    if (is_empty_list(args) || \
            (!is_empty_list(cdr(args)) && !is_empty_list(cdr(cdr(args))))) { \
        error(name " takes either one or two arguments"); \
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


#define require_character(arg, name) \
    if (!is_character(arg)) { \
        error(name " called with non-character argument"); \
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


#define require_input_port(arg, name) \
    if (!is_input_port(arg)) { \
        error(name " called with non-input-port argument"); \
    }


#define require_output_port(arg, name) \
    if (!is_output_port(arg)) { \
        error(name " called with non-output-port argument"); \
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


static object *is_input_port_proc(object *arguments)
{
    require_exactly_one(arguments, "input-port?");
    return get_boolean(is_input_port(car(arguments)));
}


static object *is_output_port_proc(object *arguments)
{
    require_exactly_one(arguments, "output-port?");
    return get_boolean(is_output_port(car(arguments)));
}


static object *is_eof_object_proc(object *arguments)
{
    require_exactly_one(arguments, "eof-object?");
    return get_boolean(is_end_of_file(car(arguments)));
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
    require_character(car(arguments), "char->integer");

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
static object *current_input_port_proc(object *arguments)
{
    require_zero(arguments, "current-input-port");
    return get_input_port();
}


static object *current_output_port_proc(object *arguments)
{
    require_zero(arguments, "current-output-port");
    return get_output_port();
}


static object *open_input_file_proc(object *arguments)
{
    require_exactly_one(arguments, "open-input-file");
    require_string(car(arguments), "open-input-file");

    return make_input_port(car(arguments)->value.string);
}


static object *open_output_file_proc(object *arguments)
{
    require_exactly_one(arguments, "open-output-file");
    require_string(car(arguments), "open-output-file");

    return make_output_port(car(arguments)->value.string);
}


static object *close_input_port_proc(object *arguments)
{
    require_exactly_one(arguments, "close-input-file");
    require_input_port(car(arguments), "close-input-file");

    close_port(car(arguments));
    return lookup_symbol("ok");
}


static object *close_output_port_proc(object *arguments)
{
    require_exactly_one(arguments, "close-output-port");
    require_output_port(car(arguments), "close-output-port");

    close_port(car(arguments));
    return lookup_symbol("ok");
}


static object *read_proc(object *arguments)
{
    require_at_most_one(arguments, "read");

    object *prev_port = get_input_port();
    object *result;

    if (is_empty_list(arguments)) {
        set_input_port(get_standard_input_port());
        result = bs_read();
    } else {
        require_input_port(car(arguments), "read");
        set_input_port(car(arguments));
        result = bs_read();
    }
        set_input_port(prev_port);
        return result;
}


static object *read_char_proc(object *arguments)
{
    require_at_most_one(arguments, "read-char");

    object *prev_port = get_input_port();
    int c;

    if (is_empty_list(arguments)) {
        set_input_port(get_standard_input_port());
        c = read_char();
    } else {
        require_input_port(car(arguments), "read-char");
        set_input_port(car(arguments));
        c = read_char();
    }
    set_input_port(prev_port);

    if (c == EOF) {
        return get_end_of_file();
    } else {
        return make_character((char)c);
    }
}


static object *peek_char_proc(object *arguments)
{
    require_at_most_one(arguments, "peek-char");

    object *prev_port = get_input_port();
    int c;

    if (is_empty_list(arguments)) {
        set_input_port(get_standard_input_port());
        c = peek_char();
    } else {
        require_input_port(car(arguments), "peek-char");
        set_input_port(car(arguments));
        c = peek_char();
    }
    set_input_port(prev_port);

    if (c == EOF) {
        return get_end_of_file();
    } else {
        return make_character((char)c);
    }
}


static object *write_proc(object *arguments)
{
    require_one_or_two(arguments, "write");

    if (is_empty_list(cdr(arguments))) {
        bs_write(car(arguments));
    } else {
        require_output_port(car(cdr(arguments)), "write");
        object *prev_port = get_output_port();
        set_output_port(car(cdr(arguments)));
        bs_write(car(arguments));
        set_output_port(prev_port);
    }

    return lookup_symbol("ok");
}


static object *write_char_proc(object *arguments)
{
    require_one_or_two(arguments, "write-char");
    require_character(car(arguments), "write-char");

    if (is_empty_list(cdr(arguments))) {
        write_output("%c", car(arguments)->value.character);
    } else {
        require_output_port(car(cdr(arguments)), "write-char");
        object *prev_port = get_output_port();
        set_output_port(car(cdr(arguments)));
        write_output("%c", car(arguments)->value.character);
        set_output_port(prev_port);
    }

    return lookup_symbol("ok");
}


static object *display_proc(object *arguments)
{
    require_one_or_two(arguments, "display");

    if (is_empty_list(cdr(arguments))) {
        display(car(arguments));
    } else {
        require_output_port(car(cdr(arguments)), "display");
        object *prev_port = get_output_port();
        set_output_port(car(cdr(arguments)));
        display(car(arguments));
        set_output_port(prev_port);
    }

    return lookup_symbol("ok");
}


static object *stdin_port_proc(object *arguments)
{
    require_zero(arguments, "stdin-port");
    return get_standard_input_port();
}


static object *stdout_port_proc(object *arguments)
{
    require_zero(arguments, "stdout-port");
    return get_standard_output_port();
}


static object *load_proc(object *arguments)
{
    require_one_or_two(arguments, "load");
    require_string(car(arguments), "load");

    char const *src_file = car(arguments)->value.string;
    object *input_port = make_input_port(src_file);
    object *prev_port = get_input_port();
    set_input_port(input_port);

    object *env;
    if (!is_empty_list(cdr(arguments))) {
        env = car(cdr(arguments));
    } else {
        env = get_global_environment();
    }

    object *result;
    object *obj = bs_read();
    while (!is_end_of_file(obj)) {
        result = bs_eval(obj, env);
        obj = bs_read();
    }

    set_input_port(prev_port);
    close_port(input_port);

    return result;
}


static object *error_proc(object *arguments)
{
    write_error("ERROR");
    if (!is_empty_list(arguments))
    {
        write_error(": ");
        set_output_port(get_error_port());
        while (!is_empty_list(arguments)) {
            display(car(arguments));
            write_error(" ");
            arguments = cdr(arguments);
        }
    }
    write_error("\n");
    exit(1);
}


/**** Eval/Apply ****/
object *apply_proc(object *arguments)
{
    (void)arguments;    // unused argument.
    error("something's real bad wrong if this function was called");
}


object *eval_proc(object *arguments)
{
    (void)arguments;    // unused argument.
    error("something's real bad wrong if this function was called");
}


static object *interaction_environment_proc(object *arguments)
{
    require_zero(arguments, "interaction-environment");
    return get_global_environment();
}


static object *null_environment_proc(object *arguments)
{
    require_zero(arguments, "null-environment");
    return make_null_environment();
}


static object *environment_proc(object *arguments)
{
    require_zero(arguments, "environment");
    object *env = make_null_environment();
    init_primitives(env);
    return env;
}


void init_primitives(object *env)
{
    defproc("eq?", eq_proc, env);
    defproc("null?", is_null_proc, env);
    defproc("boolean?", is_boolean_proc, env);
    defproc("symbol?", is_symbol_proc, env);
    defproc("integer?", is_integer_proc, env);
    defproc("char?", is_char_proc, env);
    defproc("string?", is_string_proc, env);
    defproc("pair?", is_pair_proc, env);
    defproc("list?", is_list_proc, env);
    defproc("procedure?", is_procedure_proc, env);
    defproc("input-port?", is_input_port_proc, env);
    defproc("output-port?", is_output_port_proc, env);
    defproc("eof-object?", is_eof_object_proc, env);
    defproc("+", add_proc, env);
    defproc("-", sub_proc, env);
    defproc("*", mult_proc, env);
    defproc("quotient", quotient_proc, env);
    defproc("remainder", remainder_proc, env);
    defproc("=", num_eq_proc, env);
    defproc("<", num_lt_proc, env);
    defproc(">", num_gt_proc, env);
    defproc("cons", cons_proc, env);
    defproc("car", car_proc, env);
    defproc("set-car!", set_car_proc, env);
    defproc("cdr", cdr_proc, env);
    defproc("set-cdr!", set_cdr_proc, env);
    defproc("length", length_proc, env);
    defproc("list", list_proc, env);
    defproc("char->integer", char_to_integer_proc, env);
    defproc("integer->char", integer_to_char_proc, env);
    defproc("number->string", number_to_string_proc, env);
    defproc("string->number", string_to_number_proc, env);
    defproc("symbol->string", symbol_to_string_proc, env);
    defproc("string->symbol", string_to_symbol_proc, env);
    defproc("current-input-port", current_input_port_proc, env);
    defproc("current-output-port", current_output_port_proc, env);
    defproc("open-input-file", open_input_file_proc, env);
    defproc("open-output-file", open_output_file_proc, env);
    defproc("close-input-port", close_input_port_proc, env);
    defproc("close-output-port", close_output_port_proc, env);
    defproc("read", read_proc, env);
    defproc("read-char", read_char_proc, env);
    defproc("peek-char", peek_char_proc, env);
    defproc("write", write_proc, env);
    defproc("write-char", write_char_proc, env);
    defproc("display", display_proc, env);
    defproc("stdin-port", stdin_port_proc, env);
    defproc("stdout-port", stdout_port_proc, env);
    defproc("load", load_proc, env);
    defproc("error", error_proc, env);
    defproc("apply", apply_proc, env);
    defproc("eval", eval_proc, env);
    defproc("interaction-environment", interaction_environment_proc, env);
    defproc("null-environment", null_environment_proc, env);
    defproc("environment", environment_proc, env);
}


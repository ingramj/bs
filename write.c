/* Expression printer for a Scheme-like language.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include <stdio.h>

#include "error.h"
#include "object.h"
#include "port.h"
#include "write.h"

static void write_string(object *exp);
static void write_pair(object *exp);

void bs_write(object *exp)
{
    if (exp == NULL) {
        // don't write anything for null expressions.
        return;
    }

    if (is_number(exp)) {
        write_output("%ld", exp->value.number);
    } else if (is_boolean(exp)) {
        write_output("#%c", is_false(exp) ? 'f' : 't');
    } else if (is_character(exp)) {
        write_output("#\\");
        if (exp->value.character == '\n') {
            write_output("newline");
        } else if (exp->value.character == ' ') {
            write_output("space");
        } else {
            write_output("%c", exp->value.character);
        }
    } else if (is_string(exp)) {
        write_string(exp);
    } else if (is_symbol(exp)) {
        write_output("%s", exp->value.symbol);
    } else if (is_empty_list(exp)) {
        write_output("()");
    } else if (is_pair(exp)) {
        write_output("(");
        write_pair(exp);
        write_output(")");
    } else if (is_procedure(exp)) {
        write_output("#<procedure>");
    } else if (is_input_port(exp)) {
        write_output("#<input-port>");
    } else if (is_output_port(exp)) {
        write_output("#<output-port>");
    } else {
        warn("unknown expression type");
    }
}


static void write_string(object *exp)
{
    write_output("\"");
    char const *pos = exp->value.string;
    while (*pos != '\0') {
        if (*pos == '\n') {
            write_output("\\n");
        } else if (*pos == '"') {
            write_output("\\\"");
        } else if (*pos == '\\') {
            write_output("\\\\");
        } else {
            write_output("%c", *pos);
        }
        pos++;
    }
    write_output("\"");
}


static void write_pair(object *exp)
{
    bs_write(car(exp));

    object *cdr_obj = cdr(exp);
    if (is_pair(cdr_obj)) {
        write_output(" ");
        write_pair(cdr_obj);
    } else if (is_empty_list(cdr_obj)) {
        return;
    } else {
        write_output(" . ");
        bs_write(cdr_obj);
    }
}


/* Expression printer for a Scheme-like language.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include <stdio.h>

#include "print.h"
#include "object.h"
#include "port.h"
#include "error.h"

static void write_string(object *exp);
static void write_pair(object *exp);

void bs_write(object *exp)
{
    if (exp == NULL || is_invalid(exp)) {
        // don't write anything for null expressions.
        return;
    }

    if (is_number(exp)) {
        port_printf("%ld", exp->value.number);
    } else if (is_boolean(exp)) {
        port_printf("#%c", is_false(exp) ? 'f' : 't');
    } else if (is_character(exp)) {
        port_printf("#\\");
        if (exp->value.character == '\n') {
            port_printf("newline");
        } else if (exp->value.character == ' ') {
            port_printf("space");
        } else {
            port_printf("%c", exp->value.character);
        }
    } else if (is_string(exp)) {
        write_string(exp);
    } else if (is_symbol(exp)) {
        port_printf("%s", exp->value.symbol);
    } else if (is_empty_list(exp)) {
        port_printf("()");
    } else if (is_pair(exp)) {
        port_printf("(");
        write_pair(exp);
        port_printf(")");
    } else if (is_procedure(exp)) {
        port_printf("#<procedure>");
    } else {
        warn("unknown expression type");
    }
}


static void write_string(object *exp)
{
    port_printf("\"");
    char const *pos = exp->value.string;
    while (*pos != '\0') {
        if (*pos == '\n') {
            port_printf("\\n");
        } else if (*pos == '"') {
            port_printf("\\\"");
        } else if (*pos == '\\') {
            port_printf("\\\\");
        } else {
            port_printf("%c", *pos);
        }
        pos++;
    }
    port_printf("\"");
}


static void write_pair(object *exp)
{
    bs_write(car(exp));

    object *cdr_obj = cdr(exp);
    if (is_pair(cdr_obj)) {
        port_printf(" ");
        write_pair(cdr_obj);
    } else if (is_empty_list(cdr_obj)) {
        return;
    } else {
        port_printf(" . ");
        bs_write(cdr_obj);
    }
}


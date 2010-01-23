/* Expression printer for a Scheme-like language.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include <stdio.h>

#include "print.h"
#include "object.h"
#include "file.h"
#include "error.h"

static void write_string(object *p, object *exp);
static void write_pair(object *p, object *exp);

void bs_write(object *p, object *exp)
{
    if (exp == NULL || is_invalid(exp)) {
        // don't write anything for null expressions.
        return;
    }

    // For now, just access the file pointer directly.
    FILE *out = p->value.port.file;

    if (is_number(exp)) {
        fprintf(out, "%ld", exp->value.number);
    } else if (is_boolean(exp)) {
        fprintf(out, "#%c", is_false(exp) ? 'f' : 't');
    } else if (is_character(exp)) {
        fprintf(out, "#\\");
        if (exp->value.character == '\n') {
            fprintf(out, "newline");
        } else if (exp->value.character == ' ') {
            fprintf(out, "space");
        } else {
            fprintf(out, "%c", exp->value.character);
        }
    } else if (is_string(exp)) {
        write_string(p, exp);
    } else if (is_symbol(exp)) {
        fprintf(out, "%s", exp->value.symbol);
    } else if (is_empty_list(exp)) {
        fprintf(out, "()");
    } else if (is_pair(exp)) {
        fprintf(out, "(");
        write_pair(p, exp);
        fprintf(out, ")");
    } else if (is_procedure(exp)) {
        fprintf(out, "#<procedure>");
    } else {
        warn("unknown expression type");
    }
}


static void write_string(object *p, object *exp)
{
    FILE *out = p->value.port.file;
    fprintf(out, "\"");
    char const *pos = exp->value.string;
    while (*pos != '\0') {
        if (*pos == '\n') {
            fprintf(out, "\\n");
        } else if (*pos == '"') {
            fprintf(out, "\\\"");
        } else if (*pos == '\\') {
            fprintf(out, "\\\\");
        } else {
            fprintf(out, "%c", *pos);
        }
        pos++;
    }
    fprintf(out, "\"");
}


static void write_pair(object *p, object *exp)
{
    FILE *out = p->value.port.file;
    bs_write(p, car(exp));

    object *cdr_obj = cdr(exp);
    if (is_pair(cdr_obj)) {
        fprintf(out, " ");
        write_pair(p, cdr_obj);
    } else if (is_empty_list(cdr_obj)) {
        return;
    } else {
        fprintf(out, " . ");
        bs_write(p, cdr_obj);
    }
}


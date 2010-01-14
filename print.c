/* Expression printer for a Scheme-like language.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include <stdio.h>

#include "print.h"
#include "object.h"
#include "error.h"

static void write_string(FILE *out, object *exp);
static void write_pair(FILE *out, object *exp);

void bs_write(FILE *out, object *exp)
{
    if (!exp) {
        error("unable to write expression");
    }

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
        write_string(out, exp);
    } else if (is_empty_list(exp)) {
        fprintf(out, "()");
    } else if (is_pair(exp)) {
        fprintf(out, "(");
        write_pair(out, exp);
        fprintf(out, ")");
    }else {
        error("unknown expression type");
    }
}


void write_string(FILE *out, object *exp)
{
    fprintf(out, "\"");
    char *pos = exp->value.string;
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


static void write_pair(FILE *out, object *exp)
{
    bs_write(out, exp->value.pair.car);

    object *cdr_obj = exp->value.pair.cdr;
    if (cdr_obj->type == PAIR) {
        fprintf(out, " ");
        write_pair(out, cdr_obj);
    } else if (is_empty_list(cdr_obj)) {
        return;
    } else {
        fprintf(out, " . ");
        bs_write(out, cdr_obj);
    }
}


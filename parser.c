/* Parser for a Scheme-like language.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include <stdio.h>

#include "parser.h"
#include "lexer.h"
#include "object.h"
#include "error.h"

static object empty_list = {.type = EMPTY_LIST };
static object true_object = {.type = BOOLEAN, .value.boolean = 1};
static object false_object = {.type = BOOLEAN, .value.boolean = 0};

object *bs_read(void)
{
    token *t = get_token();

    while (t->type != TOK_DONE) {
        switch (t->type) {
            case TOK_NUMBER:
                return make_number(t->value.number);
            case TOK_BOOLEAN:
                if (t->value.boolean == 0)
                    return &false_object;
                else
                    return &true_object;
            case TOK_CHARACTER:
                return make_character(t->value.character);
            case TOK_STRING:
                return make_string(t->value.string);
            case TOK_LPAREN:
                t = get_token();
                if (t->type == TOK_RPAREN) {
                    return &empty_list;
                } else {
                    error("non-empty lists are not implemented");
                }
            default:
                error("unknown token type");
        }
        t = get_token();
    }

    return NULL;
}


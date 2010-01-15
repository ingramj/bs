/* Parser for a Scheme-like language.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include <stdio.h>

#include "parser.h"
#include "lexer.h"
#include "object.h"
#include "table.h"
#include "error.h"

static object *read_pair(void);

object *bs_read(void)
{
    token *t = get_token();

    while (t->type != TOK_DONE) {
        switch (t->type) {
            case TOK_NUMBER:
                return make_number(t->value.number);
            case TOK_BOOLEAN:
                return get_boolean(t->value.boolean);
            case TOK_CHARACTER:
                return make_character(t->value.character);
            case TOK_STRING:
                return make_string(t->value.string);
            case TOK_SYMBOL:
                return make_symbol(t->value.string);
            case TOK_LPAREN:
                return read_pair();
            case TOK_QUOTE:
                return cons(lookup_symbol("quote"),
                        cons(bs_read(), get_empty_list()));
            case TOK_RPAREN:
                error("unexpected closing parenthesis");
            case TOK_DOT:
                error("dot outside of pair");
            default:
                error("unknown token type %d", t->type);
        }
        t = get_token();
    }

    return NULL;
}


static object *read_pair(void)
{
    token *t = get_token();
    if (t->type == TOK_RPAREN) {
        return get_empty_list();
    }

    push_back_token(t);

    object *car_obj = bs_read();
    object *cdr_obj;

    t = get_token();
    if (t->type == TOK_DOT) {
        cdr_obj = bs_read();

        t = get_token();
        if (t->type != TOK_RPAREN) {
            error("pair is missing a closing parenthesis");
        }
        return cons(car_obj, cdr_obj);
    } else {
        push_back_token(t);
        cdr_obj = read_pair();
        return cons(car_obj, cdr_obj);
    }
}

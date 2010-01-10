/* A lexer for a scheme-like language.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

typedef enum {
    TOK_DONE,
    TOK_NUMBER,
} token_type;

typedef struct token {
    union {
        long number;
    } value;
    struct token *next;
    token_type type;
} token;

token *get_token(FILE *in);

#endif


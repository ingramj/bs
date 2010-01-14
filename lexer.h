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
    TOK_BOOLEAN,
    TOK_CHARACTER,
    TOK_STRING,
    TOK_SYMBOL,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_DOT,
    TOK_QUOTE
} token_type;

typedef struct token {
    union {
        long number;
        int boolean;
        char character;
        char *string;
        char *symbol;
    } value;
    struct token *next;
    token_type type;
} token;

token *get_token(void);
void push_back_token(token *t);
void set_input_file(FILE *in);

#endif


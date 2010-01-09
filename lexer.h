/* A lexer for a scheme-like language.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#ifndef LEXER_H
#define LEXER_H

typedef enum {
    DONE,
    EOL,
    NUMBER,
} token_type;

typedef struct token {
    union {
        long number;
    } value;
    struct token *next;
    token_type type;
} token;

token *get_token(void);

#endif


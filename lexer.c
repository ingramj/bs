/* A lexer for a Scheme-like language.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include "gc.h"

#include "lexer.h"
#include "error.h"

 /* Overview:
  *
  * When get_token() is called, it returns the next token from the input
  * stream. When no more tokens are available, a special DONE token is
  * returned.
  *
  * Input is read in a line at a time, and the resulting tokens are stored
  * in a queue until needed.
  */


static token *alloc_token(void);
static token *add_token_to_queue(void);
static token *get_token_from_queue(FILE *in);
static inline int queue_is_empty(void);
static size_t read_line(FILE *in);
static void lex_input(FILE *in);
static size_t lex_number(size_t start);


/* Return the next token from the input stream. */
token *get_token(FILE *in)
{
    return get_token_from_queue(in);
}


/**** Token allocation and queueing. ****/
static token *queue_front = NULL;
static token *queue_back = NULL;

static inline int queue_is_empty(void)
{
    return queue_front == NULL;
}


static token *alloc_token(void)
{
    token *t = GC_MALLOC(sizeof(token));
    if (t == NULL) {
        error("unable to allocate a token:");
    }

    t->type = TOK_DONE;
    t->next = NULL;

    return t;
}


static token *add_token_to_queue(void)
{
    token *nt = alloc_token();

    if (queue_is_empty()) {
        queue_front = queue_back = nt;
    } else {
        queue_back->next = nt;
        queue_back = nt;
    }

    return queue_back;
}


static token *get_token_from_queue(FILE *in)
{
    while (queue_is_empty()) {
        lex_input(in);
    }

    token *t = queue_front;
    queue_front = t->next;
    return t;
}


/**** Input reading and buffering. ****/
#define BUFSIZE 1024
static char input_buffer[BUFSIZE];


static size_t read_line(FILE *in)
{
    char *l = fgets(input_buffer, BUFSIZE, in);
    if (l == NULL) {
        if (ferror(in)) {
            error("an error occurred while reading input:");
        } else {
            input_buffer[0] = '\0';
        }
    }

    return strlen(input_buffer);
}


/**** Lexical Analysis ****/
static void lex_input(FILE *in)
{
    size_t len = read_line(in);
    if (len == 0) {
        add_token_to_queue();
        return;
    }

    size_t pos = 0;
    char c;

    while (pos < len) {
        c = input_buffer[pos];

        if (isspace(c)) {
            pos++;
            continue;
        }

        if (isdigit(c) || c == '-') {
            pos = lex_number(pos);
            pos++;
            continue;
        } else if (c == '#') {
            if (input_buffer[pos + 1] == 'f') {
                token *t = add_token_to_queue();
                t->type = TOK_BOOLEAN;
                t->value.boolean = 0;
                pos += 2;
            } else if (input_buffer[pos + 1] == 't') {
                token *t = add_token_to_queue();
                t->type = TOK_BOOLEAN;
                t->value.boolean = 1;
                pos += 2;
            } else {
                error("expected a boolean, but was disappointed.");
            }
        } else {
            error("unrecognized character '%c'", c);
        }
        pos++;
    }
}


static size_t lex_number(size_t start)
{
    char *end;
    errno = 0;
    long num = strtol(&input_buffer[start], &end, 0);

    if (errno) {
        error("unable to read number:");
    } else if (&input_buffer[start] == end) {
        error("expected a number, but was disappointed");
    } else if (!isspace(*end)) {
        error("trailing characters after number");
    }

    token *t = add_token_to_queue();
    t->type = TOK_NUMBER;
    t->value.number = num;

    // Figure out how many characters strtol read.
    ptrdiff_t l = end - &input_buffer[start];
    assert(l > 0);

    return start + (size_t)l;
}


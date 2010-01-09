/* A lexer for a scheme-like language.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
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
static token *get_token_from_queue(void);
static inline int queue_is_empty(void);
static size_t read_line(FILE *in);
static void lex_input(void);


/* Return the next token from the input stream. */
token *get_token(void)
{
    return get_token_from_queue();
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

    t->type = DONE;
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


static token *get_token_from_queue(void)
{
    token *t = queue_front;

    if (queue_is_empty()) {
        // We should read in more input and create more tokens when this
        // happens. For now, we create a dummy token and stick in in the
        // queue;
        warn("no tokens in queue.");
        t = add_token_to_queue();
    }

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
static void lex_input(void)
{
    // TODO: STUB
}

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
static long read_line(FILE *in);
static void lex_input(FILE *in);
static long lex_number(long start);


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
static char *input_buffer = NULL;


/* Read a line of input, ending on a newline or null character. */
static long read_line(FILE *in)
{
    if (in == NULL) {
        error("null file pointer");
    }

    // reset input_buffer to a known size.
    size_t size = 128;
    input_buffer = GC_REALLOC(input_buffer, size);
    if (input_buffer == NULL) {
        error("unable to allocate input buffer:");
    }

    int c = fgetc(in);
    if (c == EOF) {
        return -1;
    }

    long bytes = 1;             // number of characters read.
    char *p = input_buffer;     // our position in the buffer.
    while (c != EOF && c != '\0') {
        if (bytes > (long)(size - 1)) {
            size = size + 128;
            input_buffer = GC_REALLOC(input_buffer, size);
            if (input_buffer == NULL) {
                error("unable to allocate input buffer:");
            }
            // input_buffer may have moved, so reset p.
            p = input_buffer + bytes - 1;
        }

        *p++ = (char) c;
        if (c == '\n') {
            break;
        }

        c = fgetc(in);
        bytes++;
    }

    *p = '\0';

    return bytes;
}


/**** Lexical Analysis ****/
static void lex_input(FILE *in)
{
    long len = read_line(in);
    if (len <= 0) {
        add_token_to_queue();
        return;
    }

    long pos = 0;
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
            char n = input_buffer[pos + 1];
            if (n == 'f' || n == 'F' || n == 't' || n == 'T') {
                token *t = add_token_to_queue();
                t->type = TOK_BOOLEAN;
                t->value.boolean = (n == 'f' || n == 'F' ? 0 : 1);
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


static long lex_number(long start)
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

    return start + l;
}


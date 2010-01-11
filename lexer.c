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

static long read_line(char const **bufptr, FILE *in);

static int peek(char const * const buffer);
static void lex_input(FILE *in);
static char const *lex_number(char const *buffer);
static char const *lex_octothorpe(char const *buffer);


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
    t->next = NULL;
    return t;
}


/**** Input reading and buffering. ****/

/* Read a line of input, ending on a newline or null character. Returns
 * the number of characters read, or -1 on EOF. */
static long read_line(char const **bufptr, FILE *in)
{
    if (in == NULL) {
        error("null file pointer");
    }

    static char *input_buffer = NULL;
    static size_t size = 128;
    if (input_buffer == NULL) {
        input_buffer = GC_REALLOC(input_buffer, size);
        if (input_buffer == NULL) {
            error("unable to allocate input buffer:");
        }
    }

    int c = fgetc(in);
    if (c == EOF) {
        *bufptr = NULL;
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

    *bufptr = input_buffer;
    return bytes;
}


/**** Lexical Analysis ****/

static int peek(char const * const buffer)
{
    if (*buffer == '\0') {
        return -1;
    } else {
        return *(buffer + 1);
    }
}


static void lex_input(FILE *in)
{
    char const * buffer = NULL;
    long len = read_line(&buffer, in);

    if (len <= 0) {
        token *t = add_token_to_queue();
        t->type = TOK_DONE;
        return;
    }

    char const * const end = buffer + len;

    char const *p = buffer;
    while (p < end) {
        if (isspace(*p)) {
            p++;
            continue;
        } else if (isdigit(*p) || *p == '-') {
            p = lex_number(p);
        } else if (*p == '#') {
            p = lex_octothorpe(p);
        } else {
            error("unrecognized character.");
        }
        p++;
    }
}


static char const *lex_number(char const *buffer)
{
    char *end;
    errno = 0;
    long num = strtol(buffer, &end, 0);

    if (errno) {
        error("unable to read number:");
    } else if (buffer == end) {
        error("expected a number, but was disappointed");
    } else if (!isspace(*end) && *end != '\0') {
        error("trailing characters after number");
    }

    token *t = add_token_to_queue();
    t->type = TOK_NUMBER;
    t->value.number = num;

    return end;
}


// The function that sounds like a cartoon villian.
static char const *lex_octothorpe(char const *buffer)
{
    int n = peek(buffer);
    if (n <= 0) {
        error("unexpected end of line");
    }

    if (n == 't' || n == 'T' || n == 'f' || n == 'F') {
        if (!isspace(peek(buffer + 1))) {
            error("trailing characters after boolean constant.");
        }
        token *t = add_token_to_queue();
        t->type = TOK_BOOLEAN;
        t->value.boolean = (n == 'f' || n == 'F') ? 0 : 1;
        return buffer + 1;
    } else {
        error("expected a boolean constant, was disappointed");
    }
}


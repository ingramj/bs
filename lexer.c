/* Lexer for a Scheme-like language.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "gc.h"

#include "lexer.h"
#include "error.h"

/**** File handling and reading ****/
static long read_line(char **bufptr);

/**** Lexical analysis ****/
static void tokenize_line(void);
static int is_delim(char c);
static token *lex_token(char const *start, char const **end);
static int lex_number(char const *start, char const **end, long *value);
static int lex_boolean(char const *start, char const **end, int *value);
static int lex_character(char const *start, char const **end, char *value);
static int lex_string(char const *start, char const **end, char **value);
static int is_initial(char c);
static int is_subsequent(char c);
static int lex_symbol(char const *start, char const **end, char **value);

/**** Token allocation and queuing ****/
static token *alloc_token(void);
static void add_token_to_queue(token *t);
static token *get_token_from_queue(void);
static inline int queue_is_empty(void);


/**** Public interface ****/
token *get_token(void)
{
    while (queue_is_empty()) {
        tokenize_line();
    }

    return get_token_from_queue();
}


/**** File handling and reading ****/
static FILE *input_file = NULL;


void set_input_file(FILE *f)
{
    if (f == NULL) {
        warn("null file pointer, defaulting to stdin");
        input_file = stdin;
    } else {
        input_file = f;
    }
}


/* Reads in a line of input, ended by a newline, null, or end of file.
 *
 * Takes the address of a pointer to const char, which is set to point
 * to the resulting null-terminated string. Returns the number of
 * characters read, or -1 on EOF.
 */
static long read_line(char **bufptr)
{
    if (input_file == NULL) {
        set_input_file(stdin);
    }

    char *input_buffer = NULL;
    size_t size = 128;
    input_buffer = GC_REALLOC(input_buffer, size);
    if (input_buffer == NULL) {
        error("unable to allocate input buffer:");
    }

    int c = fgetc(input_file);
    if (c == EOF) {
        *bufptr = NULL;
        return -1;
    }

    long bytes = 1;
    char *pos = input_buffer;
    while (c != EOF && c != '\0') {
        // resize the input buffer, if necessary.
        if (bytes > (long)(size - 1)) {
            size += 128;
            input_buffer = GC_REALLOC(input_buffer, size);
            if (input_buffer == NULL) {
                error("unable to allocate input buffer:");
            }
            pos = input_buffer + bytes - 1;
        }

        *pos++ = (char)c;
        if (c == '\n') {
            break;
        }

        c = fgetc(input_file);
        bytes++;
    }

    *pos = '\0';

    *bufptr = input_buffer;
    return bytes;
}


/**** Lexical analysis ****/
void tokenize_line(void)
{
    char *buffer;
    long len = read_line(&buffer);

    if (len < 0) {
        add_token_to_queue(alloc_token());
        return;
    }

    char const *pos = buffer;
    char const *end = NULL;
    token *t;
    while (*pos != '\0') {
        t = lex_token(pos, &end);
        if (t == NULL) break;
        add_token_to_queue(t);
        pos = end;
    }
}


static int is_delim(char c)
{
    return  isspace(c) || c == '\0' ||
            c == '(' || c == ')' ||
            c == '"';
}


/* Scan buffer for a token. Returns the first token in the buffer, or NULL if
 * none are found. The second parameter is set to the address of the character
 * just after the first token.
 */
static token *lex_token(char const *buffer, char const **end)
{
    while (isspace(*buffer)) {
        buffer++;
    }

    if (*buffer == '\0' || *buffer == ';') {
        return NULL;
    }

    token *t = alloc_token();

    if (*buffer == '(') {
        t->type = TOK_LPAREN;
        *end = buffer + 1;
        return t;
    } else if (*buffer == ')') {
        t->type = TOK_RPAREN;
        *end = buffer + 1;
        return t;
    } else if (*buffer == '.') {
        t->type = TOK_DOT;
        *end = buffer + 1;
    } else if (*buffer == '\'') {
        t->type = TOK_QUOTE;
        *end = buffer + 1;
        return t;
    } else if (lex_number(buffer, end, &t->value.number)) {
        t->type = TOK_NUMBER;
    } else if (lex_boolean(buffer, end, &t->value.boolean)) {
        t->type = TOK_BOOLEAN;
    } else if (lex_character(buffer, end, &t->value.character)) {
        t->type = TOK_CHARACTER;
    } else if (lex_string(buffer, end, &t->value.string)) {
        t->type = TOK_STRING;
    } else if (lex_symbol(buffer, end, &t->value.symbol)) {
        t->type = TOK_SYMBOL;
    } else {
        error("unable to create token from input");
    }

    if (!is_delim(**end)) {
        error("trailing characters after token");
    }

    return t;
}


/* There is a lex_X function for each token type. They all return true if the
 * string given as the first parameter matches, and false otherwise. The
 * second parameter is used to return a pointer to the next character after
 * the token that was recognized. If there was no match, it will equal start.
 * Finally, if a match was found, its value is stored in the last parameter.
 */
static int lex_number(char const *start, char const **end, long *value)
{
    if (!isdigit(*start) && *start != '-') {
        *end = start;
        return 0;
    }

    errno = 0;
    char *num_end;
    long num = strtol(start, &num_end, 0);

    if (num_end == start) {
        return 0;
    } else if (errno) {
        error("unable to read number:");
    }

    *value = num;
    *end = num_end;
    return 1;
}


static int lex_boolean(char const *start, char const **end, int *value)
{
    if (*start != '#') {
        *end = start;
        return 0;
    } else {
        char next = *(start + 1);
        if (next == 't' || next == 'T') {
            *value = 1;
        } else if (next == 'f' || next == 'F') {
            *value = 0;
        } else {
            *end = start;
            return 0;
        }
    }

    *end = start + 2;
    return 1;
}


static int lex_character(char const *start, char const **end, char *value)
{
    if (*start != '#' && *(start + 1) != '\\') {
        *end = start;
        return 0;
    }

    char const *pos = start + 2;
    if (isspace(*pos)) {
        *end = start;
        return 0;
    }

    if (*pos == 's' && !is_delim(*(pos + 1))) {
        if (strncmp(pos , "space", 4) == 0) {
            *value = ' ';
            *end = pos + 5;
            return 1;
        } else {
            *end = start;
            return 0;
        }
    } else if (*pos == 'n' && !is_delim(*(pos + 1))) {
        if (strncmp((pos), "newline", 7) == 0) {
            *value = '\n';
            *end = pos + 7;
            return 1;
        } else {
            *end = start;
            return 0;
        }
    }

    *value = *pos;
    *end = start + 3;
    return 1;
}


static int lex_string(char const *start, char const **end, char **value)
{
    if (*start != '"') {
        *end = start;
        return 0;
    }

    size_t size = 64;
    char *buffer = GC_MALLOC(size);

    char const *in_pos = start + 1;
    char *out_pos = buffer;
    size_t bytes = 0;

    while (*in_pos != '"') {
        if (*in_pos == '\0') {
            error("unterminated string constant.");
        }

        if (bytes > size - 1) {
            size += 64;
            buffer = GC_REALLOC(buffer, size);
            if (buffer == NULL) {
                error("unable to increase string buffer size:");
            }
            out_pos = buffer + bytes;
        }

        if (*in_pos == '\\') {
            in_pos++;
            if (*in_pos == 'n') {
                *out_pos = '\n';
            } else if (*in_pos == '"') {
                *out_pos = '"';
            } else if (*in_pos == '\\') {
                *out_pos = '\\';
            } else {
                error("unrecognized escape sequence: \\%c", *out_pos);
            }
            out_pos++;
            in_pos++;
            continue;
        }

        *out_pos++ = *in_pos++;
        bytes++;
    }

    *out_pos = '\0';
    *value = buffer;
    *end = in_pos + 1;
    return 1;
}


static int is_initial(char c)
{
    return isalpha(c) || c == '!' || c == '$' || c == '%' || c == '&' ||
        c == '*' || c == '/' || c == ':' || c == '<' || c == '>' ||
        c == '?' || c == '^' || c == '_' || c == '~';
}


static int is_subsequent(char c)
{
    return is_initial(c) || isdigit(c) || c == '+' ||
        c == '-' || c == '.' || c == '@';
}


static int lex_symbol(char const *start, char const **end, char **value)
{
    if (!(is_initial(*start) || ((*start == '+' ||  *start == '-') &&
                is_delim(*(start+1))))) {
        *end = start;
        return 0;
    }

    size_t size = 64;
    char *buffer = GC_MALLOC(size);

    char const *in_pos = start;
    char *out_pos = buffer;
    size_t bytes = 0;

    while (is_subsequent(*in_pos)) {
        if (bytes > size - 1) {
            size += 64;
            buffer = GC_REALLOC(buffer, size);
            if (buffer == NULL) {
                error("unable to increase symbol buffer size:");
            }
            out_pos = buffer + bytes;
        }
        *out_pos++ = *in_pos++;
        bytes++;
    }

    *out_pos = '\0';
    *value = buffer;
    *end = in_pos;
    return 1;
}


/**** Token allocation and queuing ****/
static token *queue_front = NULL;
static token *queue_back = NULL;


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


static void add_token_to_queue(token *t)
{
    if (queue_is_empty()) {
        queue_front = queue_back = t;
    } else {
        queue_back->next = t;
        queue_back = t;
    }

    t->next = NULL;
}


static token *get_token_from_queue(void)
{
    while (queue_is_empty()) {
        //lex_input();
    }

    token *t = queue_front;
    queue_front = t->next;
    t->next = NULL;
    return t;
}


void push_back_token(token *t)
{
    if (t == NULL) {
        error("null token");
    }

    t->next = queue_front;
    queue_front = t;
}

static inline int queue_is_empty(void)
{
    return queue_front == NULL;
}


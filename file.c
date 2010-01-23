/* Input file handling.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include <stdio.h>
#include "gc.h"

#include "file.h"
#include "error.h"

extern int port_is_open(object *p);
extern int port_is_closed(object *p);
extern int port_is_eof(object *p);


static object standard_input_port;
static object standard_output_port;
static object standard_error_port;

static object *current_input_port = &standard_input_port;
static object *current_output_port = &standard_output_port;
static object *current_error_port = &standard_error_port;

void init_standard_ports(void)
{
    standard_input_port.type = PORT;
    standard_input_port.value.port.mode = 0;
    standard_input_port.value.port.state = 1;
    standard_input_port.value.port.file = stdin;

    standard_output_port.type = PORT;
    standard_output_port.value.port.mode = 1;
    standard_output_port.value.port.state = 1;
    standard_output_port.value.port.file = stdout;

    standard_error_port.type = PORT;
    standard_error_port.value.port.mode = 1;
    standard_error_port.value.port.state = 1;
    standard_error_port.value.port.file = stderr;
}


object *get_standard_input_port(void)
{
    return &standard_input_port;
}


object *get_standard_output_port(void)
{
    return &standard_output_port;
}


object *get_standard_error_port(void)
{
    return &standard_error_port;
}


object *get_current_input_port(void)
{
    return current_input_port;
}


object *get_current_output_port(void)
{
    return current_output_port;
}


object *get_current_error_port(void)
{
    return current_error_port;
}


void set_current_input_port(object *p)
{
    if (is_input_port(p)) {
        current_input_port = p;
    } else {
        error("not an input port");
    }
}


void set_current_output_port(object *p)
{
    if (is_output_port(p)) {
        current_output_port = p;
    } else {
        error("not an output port");
    }
}


void set_current_error_port(object *p)
{
    if (is_output_port(p)) {
        current_error_port = p;
    } else {
        error("not an output port");
    }
}


static int is_standard_port(object *p)
{
    return p == &standard_input_port || p == &standard_output_port ||
        p == &standard_error_port;
}


void open_port(object *p, char const *file)
{
    if (is_standard_port(p)) {
        warn("ignoring attempt to reopen a standard port");
        return;
    }

    FILE *fp;
    if (p->value.port.mode == 1) {
        fp = fopen(file, "w");
    } else {
        fp = fopen(file, "r");
    }

    if (fp == NULL) {
        error("unable to open %s:", file);
    }

    p->value.port.file = fp;
    p->value.port.state = 1;
}


void close_port(object *p)
{
    if (is_standard_port(p)) {
        warn("ignoring attempt to close a standard port");
    }

    if (p->value.port.state == 0) {
        return;
    }

    fclose(p->value.port.file);
    p->value.port.state = 0;
}


int read_char(object *p)
{
    if (port_is_closed(p)) {
        error("port is closed");
    }

    int c = fgetc(p->value.port.file);
    if (c == EOF) {
        if (ferror(p->value.port.file)) {
            error("error reading from port:");
        } else {
            p->value.port.state = -1;
        }
    }

    return c;
}


/* Reads in a line of input, ended by a newline, null, or end of file.
 *
 * Takes the address of a pointer to const char, which is set to point
 * to the resulting null-terminated string. Returns the number of
 * characters read, or -1 on EOF.
 */
long read_line(object *p, char **bufptr)
{
    if (port_is_closed(p)) {
        error("port is closed");
    }

    char *input_buffer = NULL;
    size_t size = 128;
    input_buffer = GC_REALLOC(input_buffer, size);
    if (input_buffer == NULL) {
        error("unable to allocate input buffer:");
    }

    int c = read_char(p);
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

        c = read_char(p);
        bytes++;
    }

    *pos = '\0';

    *bufptr = input_buffer;
    return bytes;
}


void write_char(object *p, char c)
{
    (void)p;
    (void)c;
}


void write_line(object *p, char const *line)
{
    (void)p;
    (void)line;
}


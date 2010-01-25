/* I/O port handling.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include <stdio.h>
#include <stdarg.h>
#include "gc.h"

#include "error.h"
#include "port.h"

extern int port_is_open(object *p);
extern int port_is_closed(object *p);
extern int port_is_eof(object *p);

static object standard_input_port;
static object standard_output_port;
static object standard_error_port;

static object *input_port = &standard_input_port;
static object *output_port = &standard_output_port;
static object *error_port = &standard_error_port;


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


object *get_input_port(void)
{
    return input_port;
}


object *get_output_port(void)
{
    return output_port;
}


object *get_error_port(void)
{
    return error_port;
}


void set_input_port(object *p)
{
    if (is_input_port(p)) {
        input_port = p;
    } else {
        error("not an input port");
    }
}


void set_output_port(object *p)
{
    if (is_output_port(p)) {
        output_port = p;
    } else {
        error("not an output port");
    }
}


void set_error_port(object *p)
{
    if (is_output_port(p)) {
        error_port = p;
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


int read_char(void)
{
    if (port_is_closed(input_port)) {
        error("port is closed");
    }

    int c = fgetc(input_port->value.port.file);
    if (c == EOF) {
        if (ferror(input_port->value.port.file)) {
            error("error reading from port:");
        } else {
            input_port->value.port.state = -1;
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
long read_line(char **bufptr)
{
    if (port_is_closed(input_port)) {
        error("port is closed");
    }

    char *input_buffer = NULL;
    size_t size = 128;
    input_buffer = GC_REALLOC(input_buffer, size);
    if (input_buffer == NULL) {
        error("unable to allocate input buffer:");
    }

    int c = read_char();
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

        c = read_char();
        bytes++;
    }

    *pos = '\0';

    *bufptr = input_buffer;
    return bytes;
}


void write_output(char const * const fmt, ...)
{
    if (port_is_closed(output_port)) {
        error("port is closed");
    }

    va_list arg_list;
    va_start(arg_list, fmt);
    va_write_output(fmt, arg_list);
    va_end(arg_list);
}


void va_write_output(char const * const fmt, va_list args)
{
    if (port_is_closed(output_port)) {
        error("port is closed");
    }

    vfprintf(output_port->value.port.file, fmt, args);
}


void write_error(char const * const fmt, ...)
{
    if (port_is_closed(error_port)) {
        error("port is closed");
    }

    va_list arg_list;
    va_start(arg_list, fmt);
    va_write_error(fmt, arg_list);
    va_end(arg_list);
}


void va_write_error(char const * const fmt, va_list args)
{
    if (port_is_closed(error_port)) {
        error("port is closed");
    }

    vfprintf(error_port->value.port.file, fmt, args);
}


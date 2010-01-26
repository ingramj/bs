/* I/O port handling.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#ifndef PORT_H
#define PORT_H

#include <stdarg.h>

#include "object.h"

void init_standard_ports(void);

object *get_standard_input_port(void);
object *get_standard_output_port(void);
object *get_standard_error_port(void);

object *get_input_port(void);
object *get_output_port(void);
object *get_error_port(void);

void set_input_port(object *p);
void set_output_port(object *p);
void set_error_port(object *p);

void open_port(object *p, char const *file);
void close_port(object *p);

inline int port_is_open(object *p) { return p->value.port.state == 1; }
inline int port_is_closed(object *p) { return p->value.port.state == 0; }
inline int port_is_eof(object *p) { return p->value.port.state == -1; }

int read_char();
int peek_char();
long read_line(char **bufptr);

void write_output(char const * const fmt, ...);
void va_write_output(char const * const fmt, va_list args);
void write_error(char const * const fmt, ...);
void va_write_error(char const * const fmt, va_list args);

#endif


/* Input file handling.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#ifndef FILE_H
#define FILE_H

#include <stdio.h>

#include "object.h"

void init_standard_ports(void);

object *get_standard_input_port(void);
object *get_standard_output_port(void);
object *get_standard_error_port(void);

object *get_current_input_port(void);
object *get_current_output_port(void);
object *get_current_error_port(void);

void set_current_input_port(object *p);
void set_current_output_port(object *p);
void set_current_error_port(object *p);

void open_port(object *p, char const *file);
void close_port(object *p);

inline int port_is_open(object *p) { return p->value.port.state == 1; }
inline int port_is_closed(object *p) { return p->value.port.state == 0; }
inline int port_is_eof(object *p) { return p->value.port.state == -1; }

int read_char(object *p);
long read_line(object *p, char **bufptr);

void write_char(object *p, char c);
void write_line(object *p, char const *line);

#endif


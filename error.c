/* Error reporting facilities.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "error.h"
#include "port.h"

#ifdef DEBUG
static error_level current_level = INFO;
#else
static error_level current_level = WARNING;
#endif

static char *level_names[] = {
    "ERROR",
    "WARNING",
    "INFO"
};


void set_error_level(error_level level)
{
    if (level > INFO) {
        warn("ignoring invalid error reporting level %d", level);
        return;
    }
    current_level = level;
}


void print_error(error_level level, char const * const file, int line,
        char const * const func, char const * const fmt, ...)
{
    if (level > current_level)
        return;

    (void)fflush(get_output_port()->value.port.file);
    write_error("%s\t%s:%d:%s: ", level_names[level], file, line, func);

    va_list arg_list;
    va_start(arg_list, fmt);
    va_write_error(fmt, arg_list);
    va_end(arg_list);

    if (fmt[0] != '\0' && fmt[strlen(fmt) - 1] == ':') {
        write_error(" %s", strerror(errno));
    }

    write_error("\n");
}


/* Error reporting facilities.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#ifndef ERROR_H
#define ERROR_H

#include <stdlib.h>

typedef enum {
    ERROR,
    WARNING,
    INFO
} error_level;


#define error(...) do { \
    print_error(ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__); \
    exit(EXIT_FAILURE); \
} while (0)


#define warn(...) \
    print_error(WARNING, __FILE__, __LINE__, __func__, __VA_ARGS__)


#define info(...) \
    print_error(INFO, __FILE__, __LINE__, __func__, __VA_ARGS__)


void set_error_level(error_level level);

void print_error(error_level level, char const * const file, int line,
        char const * const func, char const * const fmt, ...);

#endif


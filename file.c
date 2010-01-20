/* Input file handling.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include <stdio.h>

#include "file.h"
#include "error.h"

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

FILE *get_input_file(void)
{
    return input_file;
}


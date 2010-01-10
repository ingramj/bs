/* Expression printer for a Scheme-like language.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#ifndef PRINT_H
#define PRINT_H

#include <stdio.h>

#include "object.h"

void bs_write(FILE *out, object *exp);

#define PRINT_H

#endif


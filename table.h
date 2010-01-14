/* Symbol table.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#ifndef TABLE_H
#define TABLE_H

#include "object.h"

object *insert_symbol(object *symbol);
object *lookup_symbol(char const *name);

#endif


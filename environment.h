/* Evaluation environments.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "object.h"

void init_environments(void);
object *get_global_environment(void);

object *lookup_variable_value(object *var, object *env);
object *set_variable_value(object *var, object *val, object *env);
void define_variable(object *var, object *val, object *env);
object *extend_environment(object *vars, object *vals, object *base_env);

#endif


/* Expression evaluator for a Scheme-like language.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#ifndef EVAL_H
#define EVAL_H

#include "object.h"

object *bs_eval(object *exp, object *env);
void init_special_forms(void);

#define EVAL_H

#endif


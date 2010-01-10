/* Expression evaluator for a Scheme-like language.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include "eval.h"
#include "object.h"
#include "error.h"

static int is_self_evaluating(object *obj);


object *bs_eval(object *exp)
{
    if (!is_self_evaluating(exp)) {
        error("unable to evaluate expression");
    }

    return exp;
}


static int is_self_evaluating(object *obj)
{
    if (obj && obj->type == NUMBER) {
        return 1;
    } else {
        return 0;
    }
}


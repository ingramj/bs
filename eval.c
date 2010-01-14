/* Expression evaluator for a Scheme-like language.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include "eval.h"
#include "table.h"
#include "object.h"
#include "error.h"

static int is_self_evaluating(object *obj);
static int is_quoted(object *obj);
static int is_tagged_list(object *obj, object *tag);


object *bs_eval(object *exp)
{
    if (is_self_evaluating(exp)) {
        return exp;
    } else if (is_quoted(exp)) {
        return car(cdr(exp));
    } else {
        warn("unable to evaluate expression");
        return NULL;
    }
}


static int is_self_evaluating(object *obj)
{
    if (obj == NULL) {
        error("null object");
    }

    if (obj->type == NUMBER || obj->type == BOOLEAN ||
            obj->type == CHARACTER || obj->type == STRING) {
        return 1;
    } else {
        return 0;
    }
}


static int is_quoted(object *exp)
{
    return is_tagged_list(exp, lookup_symbol("quote"));
}


static int is_tagged_list(object *exp, object *tag)
{
    if (is_pair(exp)) {
        object *car_obj = car(exp);
        return is_symbol(car_obj) && (car_obj == tag);
    }
    return 0;
}


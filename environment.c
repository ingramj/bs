/* Evaluation environments.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include "environment.h"
#include "error.h"
#include "eval.h"
#include "object.h"
#include "table.h"

/* Environment model:
 * An environment is a list of frames.
 * A frame is a list of bindings.
 * A binding is a (name . value) pair.
 */

static object *global_environment = NULL;


void init_global_environment(void)
{
    global_environment = make_null_environment();
}


object *get_global_environment(void) { return global_environment; }


object *make_null_environment(void)
{
    return extend_environment(get_empty_list(), get_empty_list(),
            get_empty_list());
}


static inline object *first_frame(object *env) { return car(env); }


static inline object *enclosing_environment(object *env) { return cdr(env); }


static object *make_frame(object *vars, object *vals)
{
    if (is_empty_list(vars)) {
        return get_empty_list();
    }

    object *frame = get_empty_list();
    while (!is_empty_list(vars)) {
        frame = cons(cons(car(vars), car(vals)), frame);
        vars = cdr(vars);
        vals = cdr(vals);
    }
    return frame;
}


object *lookup_variable_value(object *var, object *env)
{
    object *frame, *binding;
    while (!is_empty_list(env)) {
        frame = first_frame(env);
        while (!is_empty_list(frame)) {
            binding = car(frame);
            if (car(binding) == var) {
                return cdr(binding);
            }
            frame = cdr(frame);
        }
        env = enclosing_environment(env);
    }
    error("variable '%s' is not bound", var->value.symbol);
}


object *set_variable_value(object *var, object *val, object *env)
{
    object *frame, *binding;
    while (!is_empty_list(env)) {
        frame = first_frame(env);
        while (!is_empty_list(frame)) {
            binding = car(frame);
            if (car(binding) == var) {
                set_cdr(binding, val);
                return lookup_symbol("ok");
            }
            frame = cdr(frame);
        }
        env = enclosing_environment(env);
    }
    error("cannot set unbound variable '%s'", var->value.symbol);
}


void define_variable(object *var, object *val, object *env)
{
    object *frame = first_frame(env);
    if (is_empty_list(frame)) {
        frame = cons(cons(var, val), get_empty_list());
        set_car(env, frame);
        return;
    }
    object *binding;
    while (!is_empty_list(frame)) {
        binding = car(frame);
        if (car(binding) == var) {
            set_cdr(binding, val);
            return;
        }
        frame = cdr(frame);
    }
    frame = cons(cons(var, val), first_frame(env));
    set_car(env, frame);
    return;
}


object *extend_environment(object *vars, object *vals, object *base_env)
{
    return cons(make_frame(vars, vals), base_env);
}


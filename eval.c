/* Expression evaluator for a Scheme-like language.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include "eval.h"
#include "environment.h"
#include "table.h"
#include "object.h"
#include "error.h"


static int is_self_evaluating(object *obj);
static inline int is_variable(object *obj);
static int is_tagged_list(object *obj, object *tag);
static inline int is_quoted(object *obj);

/**** Assignment expressions ****/
static inline int is_assignment(object *exp);
static inline object *assignment_variable(object *exp);
static inline object *assignment_value(object *exp);
static object *eval_assignment(object *exp, object *env);

/**** Definition expressions ****/
static inline int is_definition(object *exp);
static inline object *definition_variable(object *exp);
static inline object *definition_value(object *exp);
static object *eval_definition(object *exp, object *env);

/**** Conditional expressions ****/
static inline int is_if(object *exp);
static inline object *if_predicate(object *exp);
static inline object *if_consequent(object *exp);
static inline object *if_alternate(object *exp);

/**** Lambda expressions ****/
static inline int is_lambda(object *exp);
static inline object *make_lambda(object *parameters, object *body);
static inline object *lambda_parameters(object *exp);
static inline object *lambda_body(object *exp);

/**** Procedure applications ****/
static inline int is_application(object *exp);
static inline object *operator(object *exp);
static inline object *operands(object *exp);
static object *eval_argument_list(object *args, object *env);


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


static inline int is_variable(object *exp)
{
    return is_symbol(exp);
}


static inline int is_quoted(object *exp)
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


/**** Assignment expressions ****/
static inline int is_assignment(object *exp)
{
    return is_tagged_list(exp, lookup_symbol("set!"));
}


static inline object *assignment_variable(object *exp)
{
    return car(cdr(exp));
}


static inline object *assignment_value(object *exp)
{
    return car(cdr(cdr(exp)));
}


static object *eval_assignment(object *exp, object *env)
{
    set_variable_value(assignment_variable(exp),
            bs_eval(assignment_value(exp), env),
            env);
    return lookup_symbol("ok");
}


/**** Definition expressions ****/
static inline int is_definition(object *exp)
{
    return is_tagged_list(exp, lookup_symbol("define"));
}


static inline object *definition_variable(object *exp)
{
    if (is_symbol(car(cdr(exp)))) {
        return car(cdr(exp));
    } else {
        return car(car(cdr(exp)));
    }
}


static inline object *definition_value(object *exp)
{
    if (is_symbol(car(cdr(exp)))) {
        return car(cdr(cdr(exp)));
    } else {
        return make_lambda(cdr(car(cdr(exp))), cdr(cdr(exp)));
    }
}


static object *eval_definition(object *exp, object *env)
{
    define_variable(definition_variable(exp),
            bs_eval(definition_value(exp), env),
            env);
    return lookup_symbol("ok");
}


/**** Conditional expressions ****/
static inline int is_if(object *exp)
{
    return is_tagged_list(exp, lookup_symbol("if"));
}


static inline object *if_predicate(object *exp)
{
    return car(cdr(exp));
}


static inline object *if_consequent(object *exp)
{
    return car(cdr(cdr(exp)));
}


static inline object *if_alternate(object *exp)
{
    object *alt_branch = cdr(cdr(cdr(exp)));
    if (is_empty_list(alt_branch)) {
        // What to return in this situation is unspecified by the standard.
        return get_boolean(0);
    } else {
        return car(alt_branch);
    }
}


/**** Lambda expressions ****/
static inline int is_lambda(object *exp)
{
    return is_tagged_list(exp, lookup_symbol("lambda"));
}


static inline object *make_lambda(object *parameters, object *body)
{
    return cons(lookup_symbol("lambda"), cons(parameters, body));
}


static inline object *lambda_parameters(object *exp)
{
    return car(cdr(exp));
}


static inline object *lambda_body(object *exp)
{
    return cdr(cdr(exp));
}


/**** Procedure applications ****/
static inline int is_application(object *exp)
{
    return is_pair(exp);
}


static inline object *operator(object *exp)
{
    return car(exp);
}


static inline object *operands(object *exp)
{
    return cdr(exp);
}


static object *eval_argument_list(object *args, object *env)
{
    if (is_empty_list(args)) {
        return get_empty_list();
    } else {
        return cons(bs_eval(car(args), env),
                eval_argument_list(cdr(args), env));
    }
}


/**** Scheme's eval procedure. ****/
void init_special_forms(void)
{
    make_symbol("quote");
    make_symbol("set!");
    make_symbol("define");
    make_symbol("ok");  // not a special form, but returned by define and set!
    make_symbol("if");
    make_symbol("lambda");
}


object *bs_eval(object *exp, object *env)
{

tailcall:
    if (is_self_evaluating(exp)) {
        return exp;
    } else if (is_variable(exp)) {
        return lookup_variable_value(exp, env);
    } else if (is_quoted(exp)) {
        return car(cdr(exp));
    } else if (is_assignment(exp)) {
        return eval_assignment(exp, env);
    } else if (is_definition(exp)) {
        return eval_definition(exp, env);
    } else if (is_if(exp)) {
        if (is_false(bs_eval(if_predicate(exp), env))) {
            exp = if_alternate(exp);
        } else {
            exp = if_consequent(exp);
        }
        goto tailcall;
    } else if (is_lambda(exp)) {
        return make_compound_proc(lambda_parameters(exp),
                lambda_body(exp),
                env);
    } else if (is_application(exp)) {
        object *procedure = bs_eval(operator(exp), env);
        object *arguments = eval_argument_list(operands(exp), env);
        if (is_primitive_proc(procedure)) {
            return (procedure->value.primitive_proc)(arguments);
        } else if (is_compound_proc(procedure)) {
            env = extend_environment(
                    procedure->value.compound_proc.parameters,
                    arguments,
                    procedure->value.compound_proc.env);
            exp = procedure->value.compound_proc.body;
            while (!is_empty_list(cdr(exp))) {
                bs_eval(car(exp), env);
                exp = cdr(exp);
            }
            exp = car(exp);
            goto tailcall;
        } else {
            error("unknown procedure type");
        }
    } else {
        error("unable to evaluate expression");
    }
}


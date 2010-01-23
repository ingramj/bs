/* Expression evaluator.
 *
 * Copyright (c) 2010 James E. Ingram
 * See the LICENSE file for terms of use.
 */

#include "eval.h"
#include "environment.h"
#include "table.h"
#include "object.h"
#include "error.h"

/**** Identification ****/
static inline int is_self_evaluating(object *exp);
static inline int is_tagged_list(object *exp, object *tag);
static inline int is_variable(object *exp);
static inline int is_quoted(object *exp);
static inline int is_assignment(object *exp);
static inline int is_definition(object *exp);
static inline int is_if(object *exp);
static inline int is_cond(object *exp);
static inline int is_begin(object *exp);
static inline int is_lambda(object *exp);
static inline int is_let(object *exp);
static inline int is_and(object *exp);
static inline int is_or(object *exp);
static inline int is_application(object *exp);


/**** Decomposition ****/
static inline object *quoted_expression(object *exp);
static inline object *assignment_variable(object *exp);
static inline object *assignment_value(object *exp);
static inline object *definition_variable(object *exp);
static inline object *definition_value(object *exp);
static inline object *if_predicate(object *exp);
static inline object *if_consequent(object *exp);
static inline object *if_alternate(object *exp);
static inline object *cond_clauses(object *exp);
static inline object *cond_predicate(object *clause);
static inline object *cond_actions(object *clause);
static inline object *begin_actions(object *exp);
static inline object *lambda_parameters(object *exp);
static inline object *lambda_body(object *exp);
static inline object *let_bindings(object *exp);
static inline object *let_body(object *exp);
static inline object *binding_variable(object *binding);
static inline object *binding_value(object *binding);
static inline object *bindings_variables(object *bindings);
static inline object *bindings_values(object *bindings);
static inline object *let_variables(object *exp);
static inline object *let_values(object *exp);
static inline object *and_tests(object *exp);
static inline object *or_tests(object *exp);
static inline object *apply_operator(object *arguments);
static inline object *apply_operands(object *arguments);
static inline object *application_operator(object *exp);
static inline object *application_operands(object *exp);


/**** Conversion ****/
static inline object *make_if(object *predicate, object *consequent,
        object *alternatives);
static inline object *sequence_to_exp(object *seq);
static object *expand_clauses(object *clauses);
static inline object *cond_to_if(object *exp);
static inline object *make_begin(object *exp);
static inline object *make_lambda(object *parameters, object *body);
static inline object *make_application(object *operator, object *operands);
static inline object *let_to_application(object *exp);
static object *prepare_apply_operands(object *arguments);


/**** Evaluation ****/
static object *eval_assignment(object *exp, object *env);
static object *eval_definition(object *exp, object *env);
static object *eval_parameters(object *parameters, object *env);

extern object *apply_proc(object *arguments);   // from primitive.c

/**** Identification ****/
static inline int is_self_evaluating(object *exp)
{
    return is_number(exp) || is_boolean(exp) || is_character(exp) ||
        is_string(exp);
}


static inline int is_tagged_list(object *exp, object *tag)
{
    if (is_list(exp)) {
        return is_symbol(car(exp)) && car(exp) == tag;
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


static inline int is_assignment(object *exp)
{
    return is_tagged_list(exp, lookup_symbol("set!"));
}


static inline int is_definition(object *exp)
{

    return is_tagged_list(exp, lookup_symbol("define"));
}


static inline int is_if(object *exp)
{
    return is_tagged_list(exp, lookup_symbol("if"));
}


static inline int is_cond(object *exp)
{
    return is_tagged_list(exp, lookup_symbol("cond"));
}


static inline int is_begin(object *exp)
{
    return is_tagged_list(exp, lookup_symbol("begin"));
}


static inline int is_lambda(object *exp)
{
    return is_tagged_list(exp, lookup_symbol("lambda"));
}


static inline int is_let(object *exp)
{
    return is_tagged_list(exp, lookup_symbol("let"));
}


static inline int is_and(object *exp)
{
    return is_tagged_list(exp, lookup_symbol("and"));
}


static inline int is_or(object *exp)
{
    return is_tagged_list(exp, lookup_symbol("or"));
}


static inline int is_application(object *exp)
{
    return is_list(exp);
}


/**** Decomposition ****/
static inline object *quoted_expression(object *exp)
{
    return car(cdr(exp));
}


static inline object *assignment_variable(object *exp)
{
    return car(cdr(exp));
}


static inline object *assignment_value(object *exp)
{
    return car(cdr(cdr(exp)));
}


static inline object *definition_variable(object *exp)
{
    return is_symbol(car(cdr(exp))) ? car(cdr(exp)) : car(car(cdr(exp)));
}


static inline object *definition_value(object *exp)
{
    return is_symbol(car(cdr(exp))) ? car(cdr(cdr(exp))) :
        make_lambda(cdr(car(cdr(exp))), cdr(cdr(exp)));
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
    object *alt = cdr(cdr(cdr(exp)));
    if (is_empty_list(alt)) {
        return get_boolean(0);
    } else {
        return car(alt);
    }
}


static inline object *cond_clauses(object *exp)
{
    return cdr(exp);
}


static inline object *cond_predicate(object *clause)
{
    return car(clause);
}


static inline object *cond_actions(object *clause)
{
    return cdr(clause);
}


static inline object *begin_actions(object *exp)
{
    return cdr(exp);
}


static inline object *lambda_parameters(object *exp)
{
    return car(cdr(exp));
}


static inline object *lambda_body(object *exp)
{
    return cdr(cdr(exp));
}


static inline object *let_bindings(object *exp)
{
    return car(cdr(exp));
}


static inline object *let_body(object *exp)
{
    return cdr(cdr(exp));
}


static inline object *binding_variable(object *binding)
{
    return car(binding);
}


static inline object *binding_value(object *binding)
{
    if (!is_empty_list(cdr(cdr(binding)))) {
        warn("ignoring extra expressions in let binding");
    }
    return car(cdr(binding));
}


static inline object *bindings_variables(object *bindings)
{
    return is_empty_list(bindings) ?
        get_empty_list() :
        cons (binding_variable(car(bindings)),
                bindings_variables(cdr(bindings)));
}


static inline object *bindings_values(object *bindings)
{
    return is_empty_list(bindings) ?
        get_empty_list() :
        cons (binding_value(car(bindings)),
                bindings_values(cdr(bindings)));
}


static inline object *let_variables(object *exp)
{
    return bindings_variables(let_bindings(exp));
}


static inline object *let_values(object *exp)
{
    return bindings_values(let_bindings(exp));
}


static inline object *and_tests(object *exp)
{
    return cdr(exp);
}


static inline object *or_tests(object *exp)
{
    return cdr(exp);
}


static inline object *apply_operator(object *arguments)
{
    return car(arguments);
}


static inline object *apply_operands(object *arguments)
{
    return prepare_apply_operands(cdr(arguments));
}


static inline object *application_operator(object *exp)
{
    return car(exp);
}


static inline object *application_operands(object *exp)
{
    return cdr(exp);
}


/**** Conversion ****/
static inline object *make_if(object *predicate, object *consequent,
        object *alternative)
{
    return cons(lookup_symbol("if"),
            cons(predicate,
                cons(consequent,
                    cons(alternative, get_empty_list()))));
}


static inline object *sequence_to_exp(object *seq)
{
    if (is_empty_list(seq)) {
        return seq;
    } else if (is_empty_list(cdr(seq))) {
        return car(seq);
    } else {
        return make_begin(seq);
    }
}


static object *expand_clauses(object *clauses)
{
    if (is_empty_list(clauses)) {
        return get_boolean(0);
    } else {
        object *first = car(clauses);
        object *rest = cdr(clauses);
        if (cond_predicate(first) == lookup_symbol("else")) {
            if (is_empty_list(rest)) {
                return sequence_to_exp(cond_actions(first));
            } else {
                warn("else clause must be last in cond expression");
                return get_invalid();
            }
        } else {
            return make_if(cond_predicate(first),
                    sequence_to_exp(cond_actions(first)),
                    expand_clauses(rest));
        }
    }
}


static inline object *cond_to_if(object *exp)
{
    return expand_clauses(cond_clauses(exp));
}


static inline object *make_begin(object *exp)
{
    return cons(lookup_symbol("begin"), exp);
}


static inline object *make_lambda(object *parameters, object *body)
{
    return cons(lookup_symbol("lambda"), cons(parameters, body));
}


static inline object *make_application(object *operator, object *operands)
{
    return cons(operator, operands);
}


static inline object *let_to_application(object *exp)
{
    return make_application(
            make_lambda(let_variables(exp),
                let_body(exp)),
            let_values(exp));
}


static object *prepare_apply_operands(object *arguments)
{
    if (is_empty_list(cdr(arguments))) {
        return car(arguments);
    } else {
        return cons(car(arguments), prepare_apply_operands(cdr(arguments)));
    }
}


/**** Evaluation ****/
static object *eval_assignment(object *exp, object *env)
{
    return set_variable_value(assignment_variable(exp),
            bs_eval(assignment_value(exp), env),
            env);
}


static object *eval_definition(object *exp, object *env)
{
    define_variable(definition_variable(exp),
            bs_eval(definition_value(exp), env),
            env);
    return lookup_symbol("ok");
}


static object *eval_parameters(object *parameters, object *env)
{
    if (is_empty_list(parameters)) {
        return get_empty_list();
    } else {
        return cons(bs_eval(car(parameters), env),
                eval_parameters(cdr(parameters), env));
    }
}


object *bs_eval(object *exp, object *env)
{
tailcall:
    if (is_self_evaluating(exp)) {
        return exp;
    } else if (is_variable(exp)) {
        return lookup_variable_value(exp, env);
    } else if (is_quoted(exp)) {
        return quoted_expression(exp);
    } else if (is_assignment(exp)) {
        return eval_assignment(exp, env);
    } else if (is_definition(exp)) {
        return eval_definition(exp, env);
    } else if (is_if(exp)) {
        if (is_true(bs_eval(if_predicate(exp), env))) {
            exp = if_consequent(exp);
        } else {
            exp = if_alternate(exp);
        }
        goto tailcall;
    } else if (is_lambda(exp)) {
        return make_compound_proc(lambda_parameters(exp),
                lambda_body(exp),
                env);
    } else if (is_begin(exp)) {
        exp = begin_actions(exp);
        if (is_empty_list(exp)) {
            warn("empty begin block");
            return get_invalid();
        }
        while (!is_empty_list(cdr(exp))) {
            bs_eval(car(exp), env);
            exp = cdr(exp);
        }
        exp = car(exp);
        goto tailcall;
    } else if (is_cond(exp)) {
        exp = cond_to_if(exp);
        goto tailcall;
    } else if (is_let(exp)) {
        exp = let_to_application(exp);
        goto tailcall;
    } else if (is_and(exp)) {
        exp = and_tests(exp);
        if (is_empty_list(exp)) {
            return get_boolean(1);
        }
        object *result;
        while (!is_empty_list(cdr(exp))) {
            result = bs_eval(car(exp), env);
            if (is_false(result)) {
                return result;
            }
            exp = cdr(exp);
        }
        exp = car(exp);
        goto tailcall;
    } else if (is_or(exp)) {
        exp = or_tests(exp);
        if (is_empty_list(exp)) {
            return get_boolean(0);
        }
        object *result;
        while (!is_empty_list(cdr(exp))) {
            result = bs_eval(car(exp), env);
            if (is_true(result)) {
                return result;
            }
            exp = cdr(exp);
        }
        exp = car(exp);
        goto tailcall;
    } else if (is_application(exp)) {
        object *procedure = bs_eval(application_operator(exp), env);
        object *parameters = eval_parameters(application_operands(exp), env);

        // handle apply specially for tailcall requirement.
        if (is_primitive_proc(procedure) &&
                procedure->value.primitive_proc == apply_proc) {
            procedure = apply_operator(parameters);
            parameters = apply_operands(parameters);
        }

        if (is_primitive_proc(procedure)) {
            return (procedure->value.primitive_proc)(parameters);
        } else if (is_compound_proc(procedure)) {
            env = extend_environment(
                    procedure->value.compound_proc.parameters,
                    parameters,
                    procedure->value.compound_proc.env);
            exp = make_begin(procedure->value.compound_proc.body);
            goto tailcall;
        } else {
            warn("unable to apply unknown procedure type");
            return get_invalid();
        }
    } else {
        warn("unable to evaluate expression");
        return get_invalid();
    }
}


void init_special_forms(void)
{
    make_symbol("quote");
    make_symbol("set!");
    make_symbol("define");
    make_symbol("ok");
    make_symbol("if");
    make_symbol("cond");
    make_symbol("else");
    make_symbol("begin");
    make_symbol("lambda");
    make_symbol("let");
    make_symbol("and");
    make_symbol("or");
}


#include "eval.h"
#include "output.h"

extern object *false, *true;
extern object *empty_list;

int self_evaluating(object *exp)
{
  return exp->type == BOOL ||
    exp->type == NUM ||
    exp->type == CHAR ||
    exp->type == STR;
}

int is_form(object *exp, char *form)
{
  
  if (exp->type != PAIR) {
    return 0;
  }

  if (car(exp)->type == SYM && car(exp) == create_symbol(form)) {
    return 1;
  }
  return 0;

}


object *quote_text(object *exp)
{
  return cadr(exp);
}

object *list_of_values(object *exp, env* e)
{
  if (exp == NULL || exp == empty_list) {
    return NULL;
  }
  return cons(eval(car(exp), e), list_of_values(cdr(exp), e));
}

object *cond_to_if(object *rest)
{
  if (caar(rest)->type == SYM &&
      strcmp(caar(rest)->data.symbol.val, "else")== 0) {
    return cadar(rest);
  }
  else {
    object *pred = caar(rest);
    object *then = cdar(rest);
    if (cdr(then) == NULL) {
      then = car(then);
    }
    else {
      then = cons(create_symbol("begin"),then);
    }
    
    if (cdr(rest) == NULL) {
      return cons(create_symbol("if"),
                  cons(pred, cons(then, NULL)));

    }
    return cons(create_symbol("if"),
                cons(pred, cons(then,
                                cons(cond_to_if(cdr(rest)), NULL))));
  }
}

object *let_vars(object *let)
{
  if (let == NULL) {
    return NULL;
  }
  return cons(caar(let), let_vars(cdr(let)));

}
object *let_vals(object *let)
{
  if (let == NULL) {
    return NULL;
  }
  return cons(cadar(let), let_vals(cdr(let)));

}

object *let_body(object *let)
{
  if (let == NULL) {
    return NULL;
  }
  return cons(car(let), let_body(cdr(let)));
  
}


object *let_to_lambda(object *let)
{

  object *vars = let_vars(car(let));
  object *vals = let_vals(car(let));
  object *body = cdr(let);
  object *lambda = cons(create_symbol("lambda"),
                        cons(vars, let_body(body)));
  return cons(lambda,vals);
  

}

object *apply(object *func, object *);

object* eval(object *exp, env *e)
{

  

 tail_call:
  if (self_evaluating(exp)) {
    return exp;
  }
  else if (exp->type == SYM) {
    object *val = lookup(exp, e);
    if (val == NULL) {
      fprintf(stderr, "Unbound variable: %s", exp->data.symbol.val);
      exit(1);
    }
    else {
      return val;
    }
  }
  else if (is_form(exp, "quote")) {
    return quote_text(exp);
  }
  else if (is_form(exp, "define")) {
    object *var = cadr(exp);
    object *val;
    if (var->type == PAIR) {
      object *func = car(var);
      object *params = cdr(var);
      object *body;
      if (params == NULL) {
        params = create_empty_list();
      }
      body = cddr(exp);
      var = func;
      val = eval(cons(create_symbol("lambda"),
                      cons(params, body)), e);
    }
    else {
      val = eval(caddr(exp),e);
    }
    insert(var, val, e);
    return create_symbol("ok");
    
  }
  else if (is_form(exp, "set!")) {
    object *val = eval(caddr(exp), e);
    object *var = cadr(exp);
    update(var, val, e);
    return create_symbol("ok");
  }
  else if (is_form(exp, "if")) {
    object *pred = cadr(exp);
    if (is_true(eval(pred, e))) {
      exp = caddr(exp);
    }
    else if (cdddr(exp) != NULL) {
      exp = cadddr(exp);
    }
    else {
      exp = false;
    }
    goto tail_call;
  }
  else if (is_form(exp, "begin")) {
    exp = cdr(exp);
    while (exp != NULL && cdr(exp) != NULL) {
      eval(car(exp), e);
      exp = cdr(exp);
    }
    /* TODO: should handle tail calls
     */
    return eval(car(exp), e);
  }
  else if (is_form(exp, "cond")) {
    return eval(cond_to_if(cdr(exp)), e);
  }
  else if (is_form(exp, "lambda")) {
    object *params = cadr(exp);
    object *body = cddr(exp);
    return create_compound_proc(params, body, e);
  }
  else if (is_form(exp, "let"))  {
    object *to_lambda = let_to_lambda(cdr(exp));
    return eval(to_lambda, e);
  }
  else if (is_form(exp, "or")) {
    object *rest = cdr(exp);
    while (rest != NULL) {
      if (is_true(eval(car(rest), e))) {
        return eval(car(rest), e);
      }
      rest = cdr(rest);
    }
    return false;
  }
  else if (is_form(exp, "and")) {
    object *rest = cdr(exp);
    object *last;
    while (rest != NULL) {
      last = eval(car(rest), e);
      if (!is_true(last)) {
        return false;
      }
      rest = cdr(rest);
    }
    return last;
  }
  else if (exp->type == PAIR) {
    object *func = eval(car(exp), e);
    return apply(func, list_of_values(cdr(exp), e));
  }
  else {
    fprintf(stderr, "Cannot evaluate expression (%d)\n", exp->type);
    return NULL;
  }

}


/* TODO: does not handle tail calls */
object* apply(object *func, object *args)
{
  if (func->type == PRIM_FUNC) {
    return func->data.primitive_func.func(args);
  }
  else if (func->type == COMP_FUNC) {
    object *params = func->data.compound_func.params;
    env *extended = malloc(sizeof(env));
    object *p, *a, *exp;
    init_env(extended);
    if (params->type != EMPTY_PAIR) {
      while (args != NULL) {
        
        p = car(params);
        a = car(args);
        insert(p, a, extended);
        args = cdr(args);
        params = cdr(params);
      }
    }

    /* TODO: could incorporate begin here. this
     is implicit begin */
    set_parent(extended, func->data.compound_func.e);
    exp = func->data.compound_func.body;
    while (cdr(exp) != NULL) {
      eval(car(exp), extended);
      exp = cdr(exp);
    }
    return eval(car(exp), extended);
  }
  else {
    fprintf(stderr, "Not a function: %d\n", func->type);
    exit(1);
  }
}

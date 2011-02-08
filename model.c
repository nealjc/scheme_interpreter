#include "model.h"

object *true;
object *false;
object *empty_list;
table sym_table;
object *quote_sym;
object *define_sym;
object *set_sym;

object *create_num(char *str)
{
  object *num = alloc_obj();

  num->type = NUM;
  num->data.number.val = atof(str);
  return num;
}

object *create_num_internal(double val)
{
  object *num = alloc_obj();

  num->type = NUM;
  num->data.number.val = val;
  return num;
}




object *create_char(char *str)
{
  object *new;
  if (strlen(str) > 1) {
    fprintf(stderr, "Invalid character literal");
    exit(1);
  }

  new = alloc_obj();
  new->type = CHAR;
  new->data.character.val = str[0];
  return new;

}
object *create_bool(char *str)
{
  if (strlen(str) > 1) {
    fprintf(stderr,"Invalid boolean literal");
    exit(1);
  }
  if (str[0] == 't') {
    return true;
  }
  else if (str[0] == 'f') {
    return false;
  }
  else {
    fprintf(stderr, "Invalid boolean literal");
    exit(1);
  }

}
object *create_string(char *str)
{
  object *new = alloc_obj();
  new->type = STR;
  new->data.string.val = malloc(strlen(str)+1);
  strcpy(new->data.string.val, str);
  return new;  

}
object *create_symbol(char *str)
{
  object *new;
  new = get_symbol(str, &sym_table);
  if (new != NULL) {
    return new;
  }
  new = alloc_obj();
  new->type = SYM;
  new->data.string.val = malloc(strlen(str)+1);
  strcpy(new->data.string.val, str);

  put_symbol(new, &sym_table);
  return new;  

}



object *cons(object *car, object *cdr)
{
  object *pair = alloc_obj();
  pair->type = PAIR;
  pair->data.pair.car = car;
  if (cdr == empty_list) {
    pair->data.pair.cdr = NULL;
  }
  else {
    pair->data.pair.cdr = cdr;
  }
  return pair;
}

object *car(object *pair)
{
  if (pair->type != PAIR) {
    fprintf(stderr, "Trying to take car of non-pair object");
    exit(1);
  }
  
  return pair->data.pair.car;
}

object *cdr(object *pair)
{
  if (pair->type != PAIR) {
    fprintf(stderr, "Trying to take cdr of non-pair object");
    exit(1);
  }

  return pair->data.pair.cdr;

}

object *create_empty_list()
{
  return empty_list;
}

int is_true(object *expr)
{
  return !is_false(expr);
}

int is_false(object *expr)
{
  return expr == false;
}


object *create_primitive_proc(object*(*fn)(object*))
{
  object *proc = alloc_obj();
  proc->type = PRIM_FUNC;
  proc->data.primitive_func.func = fn;
  return proc;
}

object *create_compound_proc(object *params, object *body, env *e)
{
  object *proc = alloc_obj();
  proc->type = COMP_FUNC;
  proc->data.compound_func.params = params;
  proc->data.compound_func.body = body;
  proc->data.compound_func.e = e;
  return proc;
}

/* ======= PROCS  ========
   (from peter michaux)
 */
object *add_proc(object *params)
{
  double sum = 0;
  object *cur;
  
  while (params != NULL) {
    cur = car(params);  
    if (cur->type != NUM) {
      fprintf(stderr, "Invalid parameter type. Expected number.");
      return NULL;
    }
    sum += cur->data.number.val;
    params = cdr(params);
  }
  return create_num_internal(sum);
}

object *sub_proc(object *params)
{
  double sum = 0;
  int first = 1;
  object *cur;
  
  while (params != NULL) {
    cur = car(params);  
    if (cur->type != NUM) {
      fprintf(stderr, "Invalid parameter type. Expected number.");
      return NULL;
    }
    if (first) {
      sum = cur->data.number.val;
      first = 0;
    }
    else {
      sum -= cur->data.number.val;
    }
    params = cdr(params);
  }
  return create_num_internal(sum);
}

object *mult_proc(object *params)
{
  double product = 1;
  object *cur;
  
  while (params != NULL) {
    cur = car(params);  
    if (cur->type != NUM) {
      fprintf(stderr, "Invalid parameter type. Expected number.");
      return NULL;
    }
    product *= cur->data.number.val;
    params = cdr(params);
  }
  return create_num_internal(product);
}

object *is_null_proc(object *arguments) {
  return car(arguments) == empty_list ? true : false;
}

object *is_boolean_proc(object *arguments) {
  return car(arguments)->type == BOOL ? true : false;
}

object *is_symbol_proc(object *arguments) {
  return car(arguments)->type == SYM ? true : false;
}

object *is_integer_proc(object *arguments) {
    return car(arguments)->type == NUM ? true : false;
}

object *is_char_proc(object *arguments) {
  return car(arguments)->type == CHAR ? true : false;
}

object *is_string_proc(object *arguments) {
  return car(arguments)->type == STR  ? true : false;
}

object *is_pair_proc(object *arguments) {
  return car(arguments)->type == PAIR ? true : false;
}

object *is_procedure_proc(object *arguments) {
  return (car(arguments)->type == PRIM_FUNC ||
          car(arguments)->type == COMP_FUNC) ? true : false;
}
/*
object *char_to_integer_proc(object *arguments) {
  return create_num(&(car(arguments)->data.character.val));
}

object *integer_to_char_proc(object *arguments) {
  return create_char((car(arguments))->data.number.val);
}

object *number_to_string_proc(object *arguments) {
    char buffer[100];

    sprintf(buffer, "%ld", (car(arguments))->data.fixnum.value);
    return make_string(buffer);
}

object *string_to_number_proc(object *arguments) {
    return make_fixnum(atoi((car(arguments))->data.string.value));
}

object *symbol_to_string_proc(object *arguments) {
    return make_string((car(arguments))->data.symbol.value);
}

object *string_to_symbol_proc(object *arguments) {
    return make_symbol((car(arguments))->data.string.value);
}
*/

object *quotient_proc(object *arguments) {
  return create_num_internal(
        ((car(arguments) )->data.number.val)/
        ((cadr(arguments))->data.number.val));
}

object *remainder_proc(object *arguments) {
  return create_num_internal(
        (int)((car(arguments))->data.number.val)%
        (int)((cadr(arguments))->data.number.val));
}

object *is_number_equal_proc(object *arguments) {
    double value;
    object *cur;
    
    value = (car(arguments))->data.number.val;

    while (arguments != NULL) {
      cur = car(arguments);  
      if (cur->type != NUM) {
        fprintf(stderr, "Invalid parameter type. Expected number.");
        return NULL;
      }
      if (cur->data.number.val != value) {
        return false;
      }
      arguments = cdr(arguments);
    }
    return true;
}

object *is_lessthan_proc(object *arguments) {
    double value;
    object *cur;
    
    value = (car(arguments))->data.number.val;
    arguments = cdr(arguments);

    while (arguments != NULL) {
      cur = car(arguments);  
      if (cur->type != NUM) {
        fprintf(stderr, "Invalid parameter type. Expected number.");
        return NULL;
      }
      if (cur->data.number.val <= value) {
        return false;
      }
      value = cur->data.number.val;
      arguments = cdr(arguments);
    }
    return true;
}

object *is_greaterthan_proc(object *arguments) {
    double value;
    object *cur;
    
    value = (car(arguments))->data.number.val;
    arguments = cdr(arguments);

    while (arguments != NULL) {
      cur = car(arguments);  
      if (cur->type != NUM) {
        fprintf(stderr, "Invalid parameter type. Expected number.");
        return NULL;
      }
      if (cur->data.number.val >= value) {
        return false;
      }
      value = cur->data.number.val;
      arguments = cdr(arguments);
    }
    return true;
}


object *cons_proc(object *arguments) {
  return cons(car(arguments), cadr(arguments));
}

object *car_proc(object *arguments) {
  return caar(arguments);
}

object *cdr_proc(object *arguments) {
  object *c = cdar(arguments);
  if (c == NULL) {
    return empty_list;
  }
  else {
    return c;
  }
}

object *set_cdr_proc(object *arguments) {
  object *pair = car(arguments);
  object *val = cadr(arguments);
  pair->data.pair.cdr = val;
  return create_symbol("ok");
}

object *set_car_proc(object *arguments) {
  object *pair = car(arguments);
  object *val = cadr(arguments);
  pair->data.pair.car = val;
  return create_symbol("ok");
}

object *list_proc(object *arguments) {
  return arguments;
}

object *is_eq_proc(object *arguments) {
  object *obj1;
  object *obj2;
  
  obj1 = car(arguments);
  obj2 = cadr(arguments);
  
  if (obj1->type != obj2->type) {
    return false;
  }
  switch (obj1->type) {
  case NUM:
    return (obj1->data.number.val == 
            obj2->data.number.val) ?
      true : false;
    break;
  case CHAR:
    return (obj1->data.character.val == 
            obj2->data.character.val) ?
      true : false;
    break;
  case STR:
    return (strcmp(obj1->data.string.val, 
                   obj2->data.string.val) == 0) ?
      true : false;
    break;
  default:
    return (obj1 == obj2) ? true : false;
  }
}


/* --------- OUTPUT procs --------- */
object* write_proc(object *args)
{

  object *to_write = car(args);
  FILE *out;
  if (cdr(args) == NULL) {
    out = stdout;
  }
  else {
    out = car(cdr(args))->data.output_port.out;
  }
  write(out, to_write);
  return create_symbol("ok");
}

object *create_output_port(FILE *f)
{
  object *new= alloc_obj();
  new->type = OUTPUT_PORT;
  new->data.output_port.out = f;
  return new;
}

object *open_output_port_proc(object *args)
{
  FILE *f;
  char *fname;

  fname = car(args)->data.string.val;
  if ( (f = fopen(fname, "w")) == NULL) {
    fprintf(stderr, "Could not open %s", fname);
    exit(1);
  }
  
  return create_output_port(f);
}

object *close_output_port_proc(object *args)
{
  FILE *f;
  f = car(args)->data.output_port.out;

  if (fclose(f) == 0) {
    return create_symbol("ok");
  }
  else {
    fprintf(stderr, "Could not close output port\n");
    exit(1);
  }
}


void init(env *e)
{

  true = alloc_obj();
  true->type = BOOL;
  true->data.boolean.val = 1;
  
  false = alloc_obj();
  false->type = BOOL;
  false->data.boolean.val = 0;

  empty_list = alloc_obj();
  empty_list->type = EMPTY_PAIR;

  init_table(&sym_table);
  quote_sym = create_symbol("quote");
  create_symbol("define");
  create_symbol("quote");
  create_symbol("begin");
  create_symbol("cond");

  /* Math */
  insert(create_symbol("+"), create_primitive_proc(&add_proc), e);
  insert(create_symbol("*"), create_primitive_proc(&mult_proc), e);
  insert(create_symbol("-"), create_primitive_proc(&sub_proc), e);
  insert(create_symbol("/"), create_primitive_proc(&quotient_proc), e);
  insert(create_symbol("remainder"), create_primitive_proc(&remainder_proc), e);
  insert(create_symbol("="), create_primitive_proc(&is_number_equal_proc), e);
  insert(create_symbol("<"), create_primitive_proc(&is_lessthan_proc), e);
  insert(create_symbol(">"), create_primitive_proc(&is_greaterthan_proc), e);

  /* predicates */
  insert(create_symbol("null?"), create_primitive_proc(&is_null_proc), e);
  insert(create_symbol("boolean?"), create_primitive_proc(&is_boolean_proc), e);
  insert(create_symbol("symbol?"), create_primitive_proc(&is_symbol_proc), e);
  insert(create_symbol("integer?"), create_primitive_proc(&is_integer_proc), e);
  insert(create_symbol("char?"), create_primitive_proc(&is_char_proc), e);
  insert(create_symbol("string?"), create_primitive_proc(&is_string_proc), e);
  insert(create_symbol("pair?"), create_primitive_proc(&is_pair_proc), e);
  insert(create_symbol("procedure?"), create_primitive_proc(&is_procedure_proc), e);

  /* lists */
  insert(create_symbol("cons"), create_primitive_proc(&cons_proc), e);
  insert(create_symbol("car"), create_primitive_proc(&car_proc), e);
  insert(create_symbol("cdr"), create_primitive_proc(&cdr_proc), e);
  insert(create_symbol("set-car!"), create_primitive_proc(&set_car_proc), e);
  insert(create_symbol("set-cdr!"), create_primitive_proc(&set_cdr_proc), e);
  insert(create_symbol("list"), create_primitive_proc(&list_proc), e);

  insert(create_symbol("eq?"), create_primitive_proc(&is_eq_proc), e);


  /* */

  insert(create_symbol("write"), create_primitive_proc(&write_proc), e);
  insert(create_symbol("open-output-port"),
         create_primitive_proc(&open_output_port_proc), e);
  insert(create_symbol("close-output-port"),
         create_primitive_proc(&close_output_port_proc), e);
  

}

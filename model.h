#ifndef MODEL_H
#define MODEL_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct obj object;

#include "symtab.h"
#include "env.h"
#include "output.h"
#include "mem.h"

typedef enum {NUM, CHAR, STR, BOOL, SYM, PAIR, EMPTY_PAIR, PRIM_FUNC,
              COMP_FUNC, OUTPUT_PORT} obj_type;

struct obj {
  obj_type type;

  union {
    struct {
      double val;
    } number;

    struct {
      char val;
    } character;

    struct {
      char *val;
    } string;

    struct {
      char *val;
    } symbol;

    struct {
      int val;
    } boolean;

    struct {
      struct obj *car;
      struct obj *cdr;
    } pair;

    struct {
      struct obj *(*func)(struct obj* args);
    } primitive_func;

    struct {
      object *params;
      object *body;
      env *e;
    } compound_func;

    struct {
      FILE *out;
    } output_port;

  } data;

} ;


object *create_num(char *str);
object *create_num_internal(double num);
object *create_char(char *str);
object *create_bool(char *str);
object *create_string(char *str);
object *create_symbol(char *str);
object *cons(object *car, object *cdr);
object *create_empty_list();
object *create_primitive_proc(object*(*fn)(object*));
object *create_compound_proc(object *params, object *body, env *e);

object *car(object *pair);
object *cdr(object *pair);

int is_true(object *);
int is_false(object *);

void init(env *);


#define caar(obj)   car(car(obj))
#define cadr(obj)   car(cdr(obj))
#define cdar(obj)   cdr(car(obj))
#define cddr(obj)   cdr(cdr(obj))
#define caaar(obj)  car(car(car(obj)))
#define caadr(obj)  car(car(cdr(obj)))
#define cadar(obj)  car(cdr(car(obj)))
#define caddr(obj)  car(cdr(cdr(obj)))
#define cdaar(obj)  cdr(car(car(obj)))
#define cdadr(obj)  cdr(car(cdr(obj)))
#define cddar(obj)  cdr(cdr(car(obj)))
#define cdddr(obj)  cdr(cdr(cdr(obj)))
#define caaaar(obj) car(car(car(car(obj))))
#define caaadr(obj) car(car(car(cdr(obj))))
#define caadar(obj) car(car(cdr(car(obj))))
#define caaddr(obj) car(car(cdr(cdr(obj))))
#define cadaar(obj) car(cdr(car(car(obj))))
#define cadadr(obj) car(cdr(car(cdr(obj))))
#define caddar(obj) car(cdr(cdr(car(obj))))
#define cadddr(obj) car(cdr(cdr(cdr(obj))))
#define cdaaar(obj) cdr(car(car(car(obj))))
#define cdaadr(obj) cdr(car(car(cdr(obj))))
#define cdadar(obj) cdr(car(cdr(car(obj))))
#define cdaddr(obj) cdr(car(cdr(cdr(obj))))
#define cddaar(obj) cdr(cdr(car(car(obj))))
#define cddadr(obj) cdr(cdr(car(cdr(obj))))
#define cdddar(obj) cdr(cdr(cdr(car(obj))))
#define cddddr(obj) cdr(cdr(cdr(cdr(obj))))


#endif 

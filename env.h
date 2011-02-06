#ifndef H_ENV
#define H_ENV

typedef struct e env;

#include "model.h"

typedef struct bnd {
  object *val;
  object *var;
  struct bnd *next;
} binding;

struct e {
  binding *head;
  struct e *parent;
};

/*  Insert into the environment, starting at the
    current frame and moving up */
object *lookup(object *, env *);
/*
  Update the binding. Will search starting
  from current frame and up
 */
void update(object *, object *, env *);

/*
  Insert a new binding in the current frame only
 */
void insert(object *, object *, env *);

void init_env(env *);

void set_parent(env *child, env *parent);


#endif

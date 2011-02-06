#include "env.h"

binding *find_binding(object *var, env *e, int use_parent)
{
  binding *cur;
  if (e->head == NULL && e->parent == NULL) {
    return NULL;
  }
  else if (e->head == NULL && use_parent) {
    return find_binding(var, e->parent, 1);
  }
  cur = e->head;
  while (cur != NULL) {
    if (cur->var == var) {
      return cur;
    }
    cur = cur->next;
  }
  if (use_parent && e->parent != NULL) {
    return find_binding(var, e->parent, 1);
  }
  else {
    return NULL;
  }


}

object *lookup(object *var, env *e)
{
  binding *b = find_binding(var, e, 1);
  if (b != NULL) {
    return b->val;
  }
  else {
    return NULL;
  }
}


void update(object *var, object *val, env *e)
{
  binding *b = find_binding(var, e, 1);
  if (b != NULL) {
    b->val = val;
    return;
  }
  fprintf(stderr, "Variable not bound");
}


void insert(object *var, object *val, env *e)
{
  binding *b = find_binding(var, e, 0);
  if (b == NULL) {
    binding *tmp = e->head;
    e->head = malloc(sizeof(binding));
    e->head->val = val;
    e->head->var = var;
    e->head->next = tmp;
  }
  else {
    b->val = val;
  }
}

void set_parent(env *child, env *parent)
{
  child->parent = parent;
}

void init_env(env *e)
{
  e->head = NULL;
  e->parent = NULL;
}

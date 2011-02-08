#ifndef MEM_H
#define MEM_H

#include "model.h"
#include "env.h"

typedef struct ms  {
  object *o;
  int used;
  struct ms *next;
  int mark;
} mem_slot;

object *alloc_obj();
void init_mem(env *global, unsigned long size);

#endif

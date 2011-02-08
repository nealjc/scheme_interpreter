#include "mem.h"

static mem_slot *free_list;
static env *root;

void init_mem(env *global, unsigned long size)
{
  unsigned long i;
  mem_slot *prev = NULL;
  root = global;
  
  for (i = 0; i < size; ++i) {
    object *new = malloc(sizeof(object));
    mem_slot *next = malloc(sizeof(mem_slot));
    if (new == NULL || next == NULL) {
      fprintf(stderr, "Out of memory\n");
      exit(1);
    }
    next->o = new;
    next->used = 0;
    next->next = NULL;
    next->mark = 0;
    if (prev == NULL) {
      free_list = next;
      prev = free_list;
    }
    else {
      prev->next = next;
      prev = next;
    }
  }

  prev->next = NULL;
}

object *alloc_obj()
{
  mem_slot *cur = free_list;
  while (cur != NULL) {
    if (!cur->used) {
      cur->used = 1;
      return cur->o;
    }
    cur = cur->next;
  }
  /*gc() */
  fprintf(stderr, "No more memory!\n");
  exit(1);
}

void gc()
{

}

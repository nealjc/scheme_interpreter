#include "symtab.h"


int find_bucket(char *val)
{
  long hval = 42;
  int i;

  /* bogus hash function... */
  for (i = 0; i < strlen(val); i++) {
    hval += (val[i] * 42.0 / 8.0);
  }
  return hval % INIT_SIZE;
}

void add_entry(int index, object *val, table *t)
{
  if (t->htable[index] == NULL) {
    t->htable[index] = malloc(sizeof(bucket));
    t->htable[index]->sym = val;
    t->htable[index]->next = NULL;
  }
  else {
    bucket *h = t->htable[index];
    while (h->next != NULL) {
      h = h->next;
    }
    h->next = malloc(sizeof(bucket));
    h->next->sym = val;
    h->next->next = NULL;
  }
  
}


object *get_symbol(char *val , table* t)
{
  int hval = find_bucket(val);
  bucket *b = t->htable[hval];
  if (b == NULL) {
    return NULL;
  }

  while (b != NULL) {
    if (strcmp(val, b->sym->data.symbol.val) == 0) {
      return b->sym;
    }
    b = b->next;
  }
  return NULL;
}

void put_symbol(object * val, table* t)
{
  add_entry(find_bucket(val->data.symbol.val), val, t);
}

void init_table(table *t)
{
  int i = 0;
  t->htable = calloc(INIT_SIZE, sizeof(bucket*));
  for (i = 0; i < INIT_SIZE; i++) {
    t->htable[i] = NULL;
  }

}

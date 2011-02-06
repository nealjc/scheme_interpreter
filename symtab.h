#ifndef SYMTAB_H
#define SYMTAB_H

typedef struct ta table;

#include "model.h"

#define INIT_SIZE 89

typedef struct b {
  object *sym;
  struct b *next;
  
} bucket;

struct ta {
  bucket **htable;
};

object *get_symbol(char *, table*);
void put_symbol(object *, table*);
void init_table(table *);

#endif



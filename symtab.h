#ifndef _H_SYMTAB
#define _H_SYMTAB

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



#include "output.h"
#include <assert.h>

void write(FILE *out, object *);

void write_pair(FILE *out, object *pair)
{
  object *cr, *cd;
  assert(pair->type = PAIR);
  
  cr = car(pair);
  cd = cdr(pair);

  write(out, cr);
  if (cd == NULL) {
    return;
  }
  else if (cd->type == PAIR) {
    fprintf(out, " ");
    write_pair(out, cd);
  }
  else {
    fprintf(out, " . ");
    write(out, cd);
  }
  

}

void show_nesting(object *pair, char *nest)
{
  object *c;
  char buf[100];
  int len;
  if (pair == NULL) {
    return ;
  }

  c = car(pair);
  printf("%s", nest);
  write(stdout, c);
  strcpy(buf, nest);
  len = strlen(buf);
  buf[len] = '\t';
  buf[len+1] = '\0';

  show_nesting(cdr(pair), buf);

}

void write(FILE *out, object *to_write)
{

  if (to_write == NULL) {
    return;
  }

  switch (to_write->type) {
    
  case NUM:
    if (to_write->data.number.val == (int)to_write->data.number.val) {
      fprintf(out,"%d\n", (int)to_write->data.number.val);
    }
    else {
      fprintf(out,"%f\n", to_write->data.number.val);
    }
    break;
  case CHAR:
    fprintf(out,"#\\%c\n", to_write->data.character.val);
    break;
  case STR:
    fprintf(out,"\"%s\"\n", to_write->data.string.val);
    break;
  case BOOL:
    fprintf(out,"#%c\n", to_write->data.boolean.val == 1 ? 't' : 'f');
    break;
  case SYM:
    fprintf(out,"%s\n", to_write->data.symbol.val);
    break;
  case PRIM_FUNC:
  case COMP_FUNC:
    fprintf(out,"#<procedure>");
    break;
  case PAIR:
    fprintf(out,"(");
    write_pair(out, to_write);
    fprintf(out,")\n");
    break;
  case EMPTY_PAIR:
    fprintf(out,"()\n");
    break;

  default:
    break;
  };
  fflush(out);

}

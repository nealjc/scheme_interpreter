#include <stdio.h>
#include <stdlib.h>
#include "parse.h"
#include "output.h"
#include "eval.h"
#include "model.h"
#include "env.h"



void repl()
{
  env environment;
  init_env(&environment);
  init(&environment);
  
  printf("^C to quit\n\n");

  while (1) {
    printf("> ");
    write(stdout, eval(parse(stdin), &environment));
  }
}

int main(int argc, char **argv)
{
  repl();
  return 0;
}

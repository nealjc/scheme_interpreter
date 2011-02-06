#include "model.h"
#include <stdio.h>

#ifndef OUT_H
#define OUT_H

void write(FILE *out, object *to_write);
void show_nesting(object *pair, char *nest);

#endif

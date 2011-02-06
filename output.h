#include "model.h"
#include <stdio.h>

#ifndef _OUT_H
#define _OUT_H

void write(FILE *out, object *to_write);
void show_nesting(object *pair, char *nest);

#endif

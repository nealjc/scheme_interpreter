CC = gcc -g
#-g -pg
CFLAGS = -c -Wall -pedantic 
#-Wno-deprecated
LDFLAGS = 
EXEC = repl

SOURCES=repl.c\
	eval.c\
	model.c\
	output.c\
	parse.c\
	symtab.c\
	env.c



OBJS = $(SOURCES:.c=.o)
INCLUDES = -I. 
CLEANFILES = *.o $(EXEC)

all: $(SOURCES) $(EXEC)
$(EXEC): $(OBJS) 
	$(CC) $(INCLUDES) $(LDFLAGS) $(OBJS) -o $@

repl.o: repl.c
	$(CC) $(INCLUDES) $(CFLAGS) repl.c 
.c.o: 
	$(CC) $(INCLUDES) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -f ${CLEANFILES}

.PHONY: check-syntax
check-syntax:
	$(CC) $(INCLUDES) -Wall -Wextra -pedantic -fsyntax-only $(CHK_SOURCES)

#include "parse.h"

#define TXT_BUF 500

/* TODO: */
extern object* quote_sym;

typedef enum {T_SYM, T_OPENP, T_CLOSEP, T_NUM, T_CHAR, T_BOOL,
              T_STRING, T_QUOTE, T_PERIOD} token_type;

typedef struct t {
  token_type type;
  char *val;
} token;


/* Allow for one token look-ahead */
static token *peeked = NULL;

/* stream the tokenizer reads from */
static FILE *tstream = NULL;

void free_token(token *t)
{
  free(t->val);
  free(t);
}

token *tokenize(FILE *f);

token *next_token()
{
  if (peeked != NULL) {
    token *ret = peeked;
    peeked = NULL;
    return ret;
  }
  return tokenize(tstream);
}

token *peek_token()
{
  if (peeked != NULL) {
    return peeked;
  }
  else {
    peeked = tokenize(tstream);
    return peeked;
  }
}

int peek(FILE *stream)
{
  int c = fgetc(stream);
  ungetc(c, stream);
  return c;
}

void skip_whitespace(FILE *stream)
{
  char next;
  while (1) {
    next = fgetc(stream);
    if (!(isspace(next))) {
      ungetc(next, stream);
      break;
    }
  }
}

int is_digit(char c)
{
  int i;
  for (i = 0; i < 10; i++) {
    if (c - '0' == i) {
      return 1;
    }
  }
  return 0;

}

int is_delim(char c)
{
  return isspace(c) || c == EOF ||
    c == '(' || c == ')' || c == '"' || c == ';';
}

int is_initial(char c)
{
  return isalpha(c) || c == '*' || c == '/' || c == '>' ||
    c == '<' || c == '=' || c == '?' || c == '!';
}

token *tokenize(FILE *stream)
{
  int next;
  token *t = NULL;
  
  skip_whitespace(stream);
  next = fgetc(stream);

  if (next == '#') {
    if (peek(stream) == '\\') {
      fgetc(stream);

      t = malloc(sizeof(token));
      t->type = T_CHAR;
      t->val = malloc(sizeof(char)*2);
      t->val[0] = fgetc(stream);
      t->val[1] = '\0';
      if (!is_delim(peek(stream))) {
        fprintf(stderr, "Invalid char literal");
        exit(1);
      }
    }
    else {
      t = malloc(sizeof(token));
      t->type = T_BOOL;
      t->val = malloc(sizeof(char)*2);
      t->val[0] = fgetc(stream);
      t->val[1] = '\0';
      
      if (!is_delim(peek(stream)) && t->val[0] != 't' &&
          t->val[0] != 'f') {
        fprintf(stderr, "Invalid bool literal");
        exit(1);
      }
    }
  }
  else if (next == '"') {
    char buf[TXT_BUF];
    int i = 0;
    
    while (peek(stream) != '"') {
      if (i >= TXT_BUF-1) {
        fprintf(stderr, "String literal too long");
        exit(1);
      }
      buf[i] = fgetc(stream);
      ++i;
    }
    fgetc(stream);
    buf[i] = '\0';

    t = malloc(sizeof(token));
    t->type = T_STRING;    
    t->val = malloc(sizeof(char)*(strlen(buf)+1));
    strcpy(t->val, buf);
  }
  else if (next == '(') {
    t = malloc(sizeof(token));
    t->type = T_OPENP;
    t->val = NULL;
  }
  else if (next == ')') {
    t = malloc(sizeof(token));
    t->type = T_CLOSEP;
    t->val = NULL;
  }
  else if (next == '\'') {
    t = malloc(sizeof(token));
    t->type = T_QUOTE;
    t->val = NULL;
  }
  else if (next == '.') {
    t = malloc(sizeof(token));
    t->type = T_PERIOD;
    t->val = NULL;
  }
  else if (is_digit(next) || (next == '-' && is_digit(peek(stream)))) {
    char buf[TXT_BUF];
    char *val;
    int i;
    int found_period = 0;
    
    buf[0] = next;
    
    for (i = 1; i < TXT_BUF-1; i++) {
      if (!is_digit(peek(stream)) && peek(stream) != '.') {
        break;
      }
      buf[i] = fgetc(stream);
      if (buf[i] == '.' && found_period == 0) {
        found_period = 1;
      }
      else if (buf[i] == '.' && found_period == 1) {
        printf("Invalid number");
        exit(1);
      }
    }
    buf[i] = '\0';
    if (!is_delim(peek(stream))) {
      fprintf(stderr, "num not followed by delim\n");
      exit(1);
    }

    t = malloc(sizeof(token));
    val = malloc(sizeof(char)*(strlen(buf)+1));
    strcpy(val, buf);
    t->type = T_NUM;
    t->val = val;
  }
  else if (is_initial(next) ||
           ((next == '+' || next == '-') &&
            is_delim(peek(stream))))  {
    char buf[TXT_BUF];
    char *val;
    int i;
    
    buf[0] = next;
    
    for (i = 1; i < TXT_BUF-1; i++) {
      char p = peek(stream);
      if (!(is_initial(p) || isdigit(p) || p == '+' || p=='-')){
        break;
      }
      buf[i] = fgetc(stream);
    }
    buf[i] = '\0';
    if (!is_delim(peek(stream))) {
      fprintf(stderr, "symbol not followed by delimiter");
      exit(1);
    }
    
    val = malloc(sizeof(char)*(strlen(buf)+1));
    strcpy(val, buf);
    t = malloc(sizeof(token));
    t->type = T_SYM;
    t->val = val;
  }
  else {
    printf("Unexpected character: '%c'\n", next);
    exit(1);
  }
  
  return t;
}

object *create_expr();

object *create_pair()
{
  token *ahead;
  object *expr, *car, *cdr;
  
  ahead = peek_token();
  if (ahead != NULL && ahead->type == T_CLOSEP) {
    /* free the close paren token */
    free_token(next_token());
    expr = create_empty_list();
    return expr;
  }
  else if (ahead ==  NULL) {
    fprintf(stderr, "Unexpected end of input");
    exit(1);
  }
  
  car = create_expr();
  ahead = peek_token();
  if (ahead != NULL && ahead->type == T_PERIOD) {
    free_token(next_token());
    cdr = create_expr();
    ahead = peek_token();
    if (ahead->type != T_CLOSEP) {
      fprintf(stderr, "Missing closed paren");
      exit(1);
    }
    free_token(next_token());
    return cons(car, cons(cdr, create_empty_list()));
    
  }
  else if (ahead == NULL) {
    fprintf(stderr, "Unexpected end of input");
    exit(1);
  }
  
  cdr = create_pair();
  return cons(car, cdr);

}

object *create_expr()
{

  token *t;
  object *expr;

  
  t = next_token();
  if (t == NULL) {
    printf("Unexpected end of input\n");
    exit(1);
  }
  switch (t->type) {
    
  case T_OPENP:
    free_token(t);
    expr = create_pair();
    break;
    
  case T_CHAR:
    expr = create_char(t->val);
    free_token(t);
    break;
    
  case T_BOOL:
    expr = create_bool(t->val);
    free_token(t);
    break;
    
  case T_STRING:
    expr = create_string(t->val);
    free_token(t);
    break;
    
  case T_QUOTE:
    free_token(t);
    expr = create_expr();
    expr = cons(quote_sym, cons(expr, create_empty_list()));
    break;

  case T_NUM:
    expr = create_num(t->val);
    free_token(t);
    break;
  case T_SYM:
    /* symbol */
    expr = create_symbol(t->val);
    free_token(t);
    break;
    
  default:
    printf("Unexpected token");
    exit(1);
  };

  return expr;
}

object* parse(FILE *stream)
{
  tstream = stream;
  return create_expr();
}



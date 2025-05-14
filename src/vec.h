#ifndef FILE_vec_h
#define FILE_vec_h

#include "lexer.h"

struct VecToken {
  struct Token *data;
  int capacity;
  int length;
};

struct VecToken *vec_token_init(int capacity);
void vec_token_push(struct VecToken *vec, struct Token token);

#endif

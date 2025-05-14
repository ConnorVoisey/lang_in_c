#include "vec.h"
#include "lexer.h"
#include <stdlib.h>

struct VecToken *vec_token_init(int capacity) {
  struct VecToken *vec = malloc(sizeof(struct VecToken));
  vec->capacity = capacity;
  vec->data = malloc(sizeof(struct Token) * capacity);
  return vec;
}

void vec_destruct(struct VecToken *vec) {
  free(vec->data);
  free(vec);
}

void vec_token_push(struct VecToken *vec, struct Token token) {
  if (vec->length == vec->capacity) {
    // need to expand capacity, for now just double
    int old_capacity = vec->capacity;
    vec->capacity *= 2;
    vec->data = realloc(vec->data, vec->capacity * sizeof(struct Token));
  }
  vec->data[vec->length] = token;
  vec->length ++;
}

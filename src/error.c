#include "error.h"
#include "mod.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

struct VecError *vec_error_init(int capacity) {
  struct VecError *vec = malloc(sizeof(struct VecError));
  vec->capacity = capacity;
  vec->data = malloc(sizeof(struct Error) * capacity);
  return vec;
}
void vec_error_push(struct VecError *vec, struct Error err) {
  if (vec->length == vec->capacity) {
    // need to expand capacity, for now just double
    vec->capacity *= 2;
    vec->data = realloc(vec->data, vec->capacity * sizeof(struct Error));
  }
  vec->data[vec->length] = err;
  vec->length++;
}

void vec_error_destruct(struct VecError *vec) {
  free(vec->data);
  free(vec);
}

#define CONTEXT_LINE_LENGTH 400
#define CONTEXT_ROWS 3
#define CONTEXT_TOTAL CONTEXT_ROWS * 2 + 1

const char *clr_red = "\033[31m";
const char *clr_green = "\033[32m";
const char *clr_yellow = "\033[33m";
const char *clr_blue = "\033[34m";
const char *clr_reset = "\033[0m";

int error_print_context(FILE *src, struct Error *err) {
  int first_row = err->row - CONTEXT_ROWS;
  if (first_row < 0) {
    first_row = 0;
  }
  int last_line_num = err->row + CONTEXT_ROWS + 1;
  int digits = 2;
  char row_buffer[CONTEXT_LINE_LENGTH] = {0};
  int curr_row = 0;
  rewind(src);
  int padding = (int)log10(last_line_num) + 1;
  while (curr_row < first_row) {
    curr_row++;
    fgets(row_buffer, CONTEXT_LINE_LENGTH, src);
  }
  for (curr_row = first_row; curr_row < err->row + 1; curr_row++) {
    fgets(row_buffer, CONTEXT_LINE_LENGTH, src);
    printf("%s%*d|%s %s", clr_blue, padding, curr_row + 1, clr_reset,
           row_buffer);
  }
  for (int i = 0; i < err->col + 3; i++) {
    putchar(' ');
  }
  for (int i = 0; i < err->len; i++) {
    printf("%s", clr_yellow);
    putchar('^');
  }
  printf(" <- here%s\n", clr_reset);
  for (int i = 0; i < CONTEXT_ROWS; i++) {
    char *get_str = fgets(row_buffer, CONTEXT_LINE_LENGTH, src);
    if (get_str == NULL)
      break;
    printf("%s%*d|%s %s", clr_blue, padding, curr_row + 1 + i, clr_reset,
           row_buffer);
  }
  return 0;
}

void mod_parser_render_errs(struct ModParser *mod_parser, FILE *src) {
  for (int i = 0; i < mod_parser->errs->length; i++) {
    struct Error err = mod_parser->errs->data[i];
    printf("\n%sError in %s: %s%s\n", clr_red, err.context, err.msg, clr_reset);
    printf("at: %lu, row: %u, col: %u\n", err.at, err.row + 1, err.col);
    error_print_context(src, &err);
  }
}

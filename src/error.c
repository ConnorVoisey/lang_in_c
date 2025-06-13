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

int error_print_context(FILE *src, struct Error *err, FILE *out_stream) {
  int first_row = err->row - CONTEXT_ROWS;
  if (first_row < 0) {
    first_row = 0;
  }
  int last_line_num = err->row + CONTEXT_ROWS + 1;
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
    fprintf(out_stream, "%s| %s%*d|%s %s", clr_red, clr_blue, padding,
            curr_row + 1, clr_reset, row_buffer);
  }
  fprintf(out_stream, "%s| %s", clr_red, clr_reset);
  for (int i = 0; i < err->col + 3; i++) {
    fputc(' ', out_stream);
  }
  for (int i = 0; i < err->len; i++) {
    fprintf(out_stream, "%s", clr_yellow);
    fputc('^', out_stream);
  }
  fprintf(out_stream, " <- here%s\n", clr_reset);
  for (int i = 0; i < CONTEXT_ROWS; i++) {
    char *get_str = fgets(row_buffer, CONTEXT_LINE_LENGTH, src);
    if (get_str == NULL)
      break;
    fprintf(out_stream, "%s| %s%*d|%s %s", clr_red, clr_blue, padding,
            curr_row + 1, clr_reset, row_buffer);
  }
  return 0;
}

void mod_parser_render_errs(struct ModParser *mod_parser, FILE *src,
                            FILE *out_stream) {
  for (int i = 0; i < mod_parser->errs->length; i++) {
    struct Error err = mod_parser->errs->data[i];
    fprintf(out_stream, "\n%s| Error in %s: %s%s\n", clr_red, err.context,
            err.msg, clr_reset);
    fprintf(out_stream, "%s| %sat: %lu, row: %u, col: %u\n", clr_red, clr_reset,
            err.at, err.row + 1, err.col);
    error_print_context(src, &err, out_stream);
  }
}

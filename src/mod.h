#ifndef FILE_mod_h
#define FILE_mod_h

#include "arena.h"
#include "lexer.h"
#include "vec.h"

struct ModParser {
  struct Arena arena;
  struct VecToken *tokens;
  struct VecError *errs;
};

void mod_parser_render_errs(struct ModParser *vec, FILE *src, FILE *out_stream);
int get_string_size(const char *fmt, ...);
#endif

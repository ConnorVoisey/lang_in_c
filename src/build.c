#include "arena.h"
#include "error.h"
#include "lexer.h"
#include "mod.h"
#include "parse/parse.h"
#include "vec.h"
#include <stdio.h>
#include <sys/resource.h>
#include <time.h>
#define FILE_PATH "examples/lang/hello.lang"

int build() {
  struct rusage start, end;
  getrusage(RUSAGE_SELF, &start);

  FILE *src_file = fopen(FILE_PATH, "r");
  if (src_file == NULL) {
    fprintf(stderr, "Failed to open file path: %s\n", FILE_PATH);
    return 1;
  }

  struct Arena *arena = arena_create(sizeof(char) * 10000);
  struct ModParser mod_parser = {
      .arena = *arena,
      .tokens = vec_token_init(300),
      .errs = vec_error_init(4),
  };

  lex_file(src_file, &mod_parser);
  if (mod_parser.errs->length != 0) {
    mod_parser_render_errs(&mod_parser, src_file);
    return 1;
  }

  parse_tokens(&mod_parser);
  getrusage(RUSAGE_SELF, &end);
  double duration_ns =
      (double)(end.ru_utime.tv_sec - start.ru_utime.tv_sec) * 1000000 +
      (double)(end.ru_utime.tv_usec - start.ru_utime.tv_usec);

  printf("{\"msg\": \"finished build\",\"duration_ns\": %.2f}\n", duration_ns);
  return 0;
}

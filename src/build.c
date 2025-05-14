#include "arena.h"
#include "lexer.h"
#include "mod.h"
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
  struct ModParser mod_parser = {.arena = *arena,
                                 .tokens = vec_token_init(300)};
  lex_file(src_file, &mod_parser);
  getrusage(RUSAGE_SELF, &end);
  double duration_ns =
      (double)(end.ru_utime.tv_sec - start.ru_utime.tv_sec) * 1000000 +
      (double)(end.ru_utime.tv_usec - start.ru_utime.tv_usec);

  printf("{\"msg\": \"finished build\",\"duration_ns\": %.2f}\n", duration_ns);
  return 0;
}

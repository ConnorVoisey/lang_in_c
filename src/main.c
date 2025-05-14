#include "build.h"
#include <stdio.h>
#include <string.h>

void print_usage(FILE *out_stream) {
  fprintf(stderr, "supported commands are build, e.g.\n\nbuild\n");
}

int parse_cmd(int arg_count, char **args) {
  // the only supported command is build
  if (arg_count != 2) {
    print_usage(stderr);
    return 1;
  }

  if (strcmp(args[1], "build") == 0) {
    return build();
  }

  print_usage(stderr);
  return 1;
}

int main(int arg_count, char **args) { return parse_cmd(arg_count, args); }

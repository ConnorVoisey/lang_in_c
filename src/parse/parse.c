#include "parse.h"

int parse_import(struct ModParser *mod_parser, unsigned long *at) {
  puts("parse_import");
  return 0;
}
int parse_extern(struct ModParser *mod_parser, unsigned long *at) {
  puts("parse_exter");
  return 0;
}
int parse_struct(struct ModParser *mod_parser, unsigned long *at) {
  puts("parse_struct");
  return 0;
}
int parse_fn(struct ModParser *mod_parser, unsigned long *at) {
  puts("parse_fn");
  return 0;
}
int parse_statement(struct ModParser *mod_parser, unsigned long *at) {
  puts("parse_statement");
  return 0;
}
int parse_expr(struct ModParser *mod_parser, unsigned long *at) {
  puts("parse_expr");
  return 0;
}

int parse_tokens(struct ModParser *mod_parser) {
  unsigned long at = -1;
  while (1) {
    at++;
    if (at >= mod_parser->tokens->length) {
      break;
    }
    struct Token token = mod_parser->tokens->data[at];
    printf("token kind: %d, row: %lu, col: %lu\n", token.kind, token.row,
           token.col);
    switch (token.kind) {
    case TokenType_ImportKeyWord: {
      parse_import(mod_parser, &at);
      break;
    }
    default:
      fprintf(stderr, "parse_tokens unhandled base token: %d\n", token.kind);
      return 1;
    };
  }
  return 0;
}

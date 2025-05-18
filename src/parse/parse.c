#include "parse.h"
#include "../error.h"
#include <stdio.h>

int parse_import_group(struct ModParser *mod_parser, unsigned long *at) {
  struct VecToken *tokens = mod_parser->tokens;
  struct Token token = {0};
  puts("parse import group");
  while (1) {
    (*at)++;
    printf("at: %lu\n", *at);
    if (*at >= tokens->length) {
      vec_error_push(mod_parser->errs,
                     (struct Error){
                         .at = token.starts_at,
                         .row = token.row,
                         .col = token.col,
                         .msg = "Unexpected end of input in inport statement, "
                                "try closing the `{` with `}`",
                         .context = "parse imports",
                         .len = token.len,
                     });
      return 1;
    }
    token = tokens->data[*at];
    printf("token at: %u, kind %d\n", token.starts_at, token.kind);
    switch (token.kind) {
    case TokenType_CurlyBracketClose: {
      printf("token is `}` at: %u\n", token.starts_at);
      (*at)++;
      return 0;
      break;
    }
    case TokenType_Comma: {
      printf("token is comma at: %u\n", token.starts_at);
      break;
    }
    case TokenType_Divide: {
      printf("token is divide at: %u\n", token.starts_at);
      break;
    }
    case TokenType_Ident: {
      printf("token is ident at: %u\n", token.starts_at);
      break;
    }
    default: {
      vec_error_push(mod_parser->errs,
                     (struct Error){
                         .at = token.starts_at,
                         .row = token.row,
                         .col = token.col,
                         .msg = "Unexpected token, expected ident",
                         .context = "parse import group",
                         .len = token.len,
                     });
      return 1;
    }
    }
    printf("token at: %u\n", token.starts_at);
  }
  puts("parse_import");
  return 0;
}
int parse_import(struct ModParser *mod_parser, unsigned long *at) {
  struct VecToken *tokens = mod_parser->tokens;
  struct Token token = {0};
  (*at)++;
  if (*at >= tokens->length) {
    vec_error_push(mod_parser->errs,
                   (struct Error){
                       .at = token.starts_at,
                       .row = token.row,
                       .col = token.col,
                       .msg = "Unexpected end of input in inport statement, "
                              "try closing the `{` with `}`",
                       .context = "parse imports",
                       .len = token.len,
                   });
    return 1;
  }
  token = tokens->data[*at];
  if (token.kind != TokenType_CurlyBracketOpen) {
    vec_error_push(mod_parser->errs,
                   (struct Error){
                       .at = token.starts_at,
                       .row = token.row,
                       .col = token.col,
                       .msg = "Expected `{` after `import` keyword "
                              "try adding `{` then a comma separated list of "
                              "imports or nothing then `}`",
                       .context = "parse imports",
                       .len = token.len,
                   });
    return 1;
  }

  parse_import_group(mod_parser, at);
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

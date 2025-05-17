#include "lexer.h"
#include "arena.h"
#include "error.h"
#include "mod.h"
#include <ctype.h>
#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

char *keywords_from_str(enum TokenType token_type) {
  switch (token_type) {
  case TokenType_ImportKeyWord:
    return "import";
  case TokenType_ExternKeyWord:
    return "extern";
  case TokenType_ReturnKeyWord:
    return "return";
  case TokenType_LetKeyWord:
    return "let";
  case TokenType_StructKeyWord:
    return "struct";
  case TokenType_PubKeyWord:
    return "pub";
  case TokenType_FnKeyWord:
    return "fn";
  case TokenType_HeadersKeyWord:
    return "headers";
  case TokenType_DefsKeyWord:
    return "defs";
  default:
    return "Unknown";
  }
}
char *token_type_to_str(enum TokenType token_type) {
  switch (token_type) {
  case TokenType_ImportKeyWord:
    return "ImportKeyWord";
  case TokenType_ExternKeyWord:
    return "ExternKeyWord";
  case TokenType_ReturnKeyWord:
    return "ReturnKeyWord";
  case TokenType_LetKeyWord:
    return "LetKeyWord";
  case TokenType_StructKeyWord:
    return "StructKeyWord";
  case TokenType_PubKeyWord:
    return "PubKeyWord";
  case TokenType_FnKeyWord:
    return "FnKeyWord";
  case TokenType_HeadersKeyWord:
    return "HeadersKeyWord";
  case TokenType_DefsKeyWord:
    return "DefsKeyWord";
  case TokenType_Int:
    return "Int";
  case TokenType_Str:
    return "Str";
  case TokenType_Ident:
    return "Ident";
  case TokenType_BracketOpen:
    return "BracketOpen";
  case TokenType_BracketClose:
    return "BracketClose";
  case TokenType_SquareBracketOpen:
    return "SquareBracketOpen";
  case TokenType_SquareBracketClose:
    return "SquareBracketClose";
  case TokenType_CurlyBracketOpen:
    return "CurlyBracketOpen";
  case TokenType_CurlyBracketClose:
    return "CurlyBracketClose";
  case TokenType_Comma:
    return "Comma";
  case TokenType_Dot:
    return "Dot";
  case TokenType_Amp:
    return "Amp";
  case TokenType_Plus:
    return "Plus";
  case TokenType_Minus:
    return "Minus";
  case TokenType_Times:
    return "Times";
  case TokenType_Divide:
    return "Divide";
  case TokenType_Assign:
    return "Assign";
  case TokenType_Equiv:
    return "Equiv";
  case TokenType_Lt:
    return "LessThan";
  case TokenType_LtEq:
    return "LessThanEq";
  case TokenType_Gt:
    return "GreaterThan";
  case TokenType_GtEq:
    return "GreaterThanEq";
  default:
    fprintf(stderr, "called token_type_to_str with unknown token type: %d\n",
            token_type);
    exit(1);
  }
}
char *arena_str_to_str(struct ModParser *mod_parser,
                       struct ArenaPointer *str_ptr) {
  if (mod_parser == NULL || str_ptr == NULL) {
    return NULL;
  }
  return mod_parser->arena.start + str_ptr->offset;
}

const int KeyWords[] = {
    TokenType_ImportKeyWord, TokenType_ExternKeyWord,  TokenType_ReturnKeyWord,
    TokenType_LetKeyWord,    TokenType_StructKeyWord,  TokenType_PubKeyWord,
    TokenType_FnKeyWord,     TokenType_HeadersKeyWord, TokenType_DefsKeyWord,
};
#define KEYWORD_COUNT (sizeof(KeyWords) / sizeof(*KeyWords))

int token_to_json(struct ModParser *mod_parser, FILE *out_stream,
                  struct Token *token) {
  if (token == NULL) {
    return -1;
  }
  json_t *json_contents = {0};
  json_contents = json_pack("s", "initial");
  switch (token->kind) {
  case TokenType_ImportKeyWord:
  case TokenType_ExternKeyWord:
  case TokenType_ReturnKeyWord:
  case TokenType_LetKeyWord:
  case TokenType_StructKeyWord:
  case TokenType_PubKeyWord:
  case TokenType_FnKeyWord:
  case TokenType_HeadersKeyWord:
  case TokenType_DefsKeyWord:
  case TokenType_BracketOpen:
  case TokenType_BracketClose:
  case TokenType_SquareBracketOpen:
  case TokenType_SquareBracketClose:
  case TokenType_CurlyBracketOpen:
  case TokenType_CurlyBracketClose:
  case TokenType_Comma:
  case TokenType_Dot:
  case TokenType_Amp:
  case TokenType_Plus:
  case TokenType_Minus:
  case TokenType_Times:
  case TokenType_Divide:
  case TokenType_Assign:
  case TokenType_Equiv:
  case TokenType_Lt:
  case TokenType_LtEq:
  case TokenType_Gt:
  case TokenType_GtEq:
    json_contents = json_pack("n");
    break;
  case TokenType_Int:
    json_contents = json_pack("i", token->contents.int_val);
    break;
  case TokenType_Str:
    json_contents = json_pack(
        "s", arena_str_to_str(mod_parser, &token->contents.arena_pointer));
    break;
  case TokenType_Ident:
    json_contents = json_pack(
        "s", arena_str_to_str(mod_parser, &token->contents.arena_pointer));
    break;
  default:
    json_contents = json_pack("s", "UNKNOWN");
  }
  int print_res = json_dumpf(
      json_pack("{s: i, s: I, s: I, s: i, s: s, s, o}\n", "starts_at",
                token->starts_at, "row", (long long)token->row, "col",
                (long long)token->col, "len", token->len, "kind",
                token_type_to_str(token->kind), "contents", json_contents),
      out_stream, 0);
  puts("");
  return 0;
}

int lex_file(FILE *in_stream, struct ModParser *mod_parser) {
  // for simplicity, read the entire src file to a string,
  // this wont scale but it will work for now
  fseek(in_stream, 0, SEEK_END);
  long int file_size = ftell(in_stream);
  char *src_code = malloc(file_size);
  fseek(in_stream, 0, SEEK_SET);
  fread(src_code, file_size, 1, in_stream);
  int cur_char = 1;
  uint at = -1;
  unsigned long row = 0;
  unsigned long col = -1;
  while (cur_char != '\0') {
    at++;
    col++;
    cur_char = src_code[at];
    if (cur_char == '\0')
      break;
    struct Token tok = {.starts_at = at,
                        .row = row,
                        .col = col,
                        .len = 1,
                        tok.kind = TokenType_BracketOpen,
                        tok.contents = (union TokenContents){.null = NULL}};
    switch (cur_char) {
    case '\n':
      row++;
      col = 0;
      continue;
    case '(':
      tok.kind = TokenType_BracketOpen;
      token_to_json(mod_parser, stdout, &tok);
      vec_token_push(mod_parser->tokens, tok);
      continue;
    case ')':
      tok.kind = TokenType_BracketClose;
      token_to_json(mod_parser, stdout, &tok);
      vec_token_push(mod_parser->tokens, tok);
      continue;
    case '[':
      tok.kind = TokenType_SquareBracketOpen;
      token_to_json(mod_parser, stdout, &tok);
      vec_token_push(mod_parser->tokens, tok);
      continue;
    case ']':
      tok.kind = TokenType_SquareBracketClose;
      token_to_json(mod_parser, stdout, &tok);
      vec_token_push(mod_parser->tokens, tok);
      continue;
    case '{':
      tok.kind = TokenType_CurlyBracketOpen;
      token_to_json(mod_parser, stdout, &tok);
      vec_token_push(mod_parser->tokens, tok);
      continue;
    case '}':
      tok.kind = TokenType_CurlyBracketClose;
      token_to_json(mod_parser, stdout, &tok);
      vec_token_push(mod_parser->tokens, tok);
      continue;
    case ',':
      tok.kind = TokenType_Comma;
      token_to_json(mod_parser, stdout, &tok);
      vec_token_push(mod_parser->tokens, tok);
      continue;
    case '.':
      tok.kind = TokenType_Dot;
      token_to_json(mod_parser, stdout, &tok);
      vec_token_push(mod_parser->tokens, tok);
      continue;
    case '&':
      tok.kind = TokenType_Amp;
      token_to_json(mod_parser, stdout, &tok);
      vec_token_push(mod_parser->tokens, tok);
      continue;
    case '+':
      tok.kind = TokenType_Plus;
      token_to_json(mod_parser, stdout, &tok);
      vec_token_push(mod_parser->tokens, tok);
      continue;
    case '-':
      tok.kind = TokenType_Minus;
      token_to_json(mod_parser, stdout, &tok);
      vec_token_push(mod_parser->tokens, tok);
      continue;
    case '*':
      tok.kind = TokenType_Times;
      token_to_json(mod_parser, stdout, &tok);
      vec_token_push(mod_parser->tokens, tok);
      continue;
    case '/':
      tok.kind = TokenType_Divide;
      token_to_json(mod_parser, stdout, &tok);
      vec_token_push(mod_parser->tokens, tok);
      continue;
    case '=': {
      char next_char = src_code[at + 1];
      if (next_char == '=') {
        at++;
        col++;
        tok.kind = TokenType_Equiv;
      } else {
        tok.kind = TokenType_Assign;
      }
      token_to_json(mod_parser, stdout, &tok);
      vec_token_push(mod_parser->tokens, tok);
      continue;
    }
    case '<': {
      char next_char = src_code[at + 1];
      if (next_char == '=') {
        at++;
        col++;
        tok.kind = TokenType_LtEq;
      } else {
        tok.kind = TokenType_Lt;
      }
      token_to_json(mod_parser, stdout, &tok);
      vec_token_push(mod_parser->tokens, tok);
      continue;
    }
    case '>': {
      char next_char = src_code[at + 1];
      if (next_char == '=') {
        at++;
        col++;
        tok.kind = TokenType_GtEq;
      } else {
        tok.kind = TokenType_Gt;
      }
      token_to_json(mod_parser, stdout, &tok);
      vec_token_push(mod_parser->tokens, tok);
      continue;
    }
    }
    if (cur_char == '\n') {

      continue;
    } else if (isspace(cur_char)) {
      continue;
    }
    if (isdigit(cur_char)) {
      int str_start = at;
      unsigned long col_start = col;
      int str_len = 0;
      int str_char = 0;
      int arena_start_offset = mod_parser->arena.offset;

      ((char *)(mod_parser->arena.start + mod_parser->arena.offset))[0] =
          cur_char;
      mod_parser->arena.offset += sizeof(char);

      while (str_char != EOF) {
        str_len++;
        str_char = src_code[at + str_len];
        if (str_char == '_') {
          continue;
        }
        if (!isdigit(str_char)) {
          break;
        }

        ((char *)(mod_parser->arena.start + mod_parser->arena.offset))[0] =
            str_char;
        mod_parser->arena.offset += sizeof(char);
      }
      at += str_len - 1;
      col += str_len - 1;

      struct ArenaPointer str = {
          .offset = arena_start_offset,
          .len = str_len * sizeof(char),
      };
      char *str_ptr = arena_get_pointer(&mod_parser->arena, str.offset);
      int int_val = strtol(str_ptr, NULL, 10);
      struct Token tok = {.starts_at = str_start,
                          .row = row,
                          .col = col_start,
                          .len = str_len,
                          tok.kind = TokenType_Int,
                          tok.contents =
                              (union TokenContents){.int_val = int_val}};
      token_to_json(mod_parser, stdout, &tok);
      vec_token_push(mod_parser->tokens, tok);
      continue;
    }
    if (isalpha(cur_char)) {
      int str_start = at;
      unsigned long col_start = col;
      int str_len = 0;
      int str_char = 0;
      int arena_start_offset = mod_parser->arena.offset;

      ((char *)(mod_parser->arena.start + mod_parser->arena.offset))[0] =
          cur_char;
      mod_parser->arena.offset += sizeof(char);

      while (str_char != EOF) {
        str_len++;
        str_char = src_code[at + str_len];
        if (!isalpha(str_char) && !isdigit(str_char) && str_char != '_') {
          break;
        }

        ((char *)(mod_parser->arena.start + mod_parser->arena.offset))[0] =
            str_char;
        mod_parser->arena.offset += sizeof(char);
      }
      at += str_len - 1;
      col += str_len - 1;
      struct ArenaPointer str = {
          .offset = arena_start_offset,
          .len = str_len * sizeof(char),
      };
      struct Token tok = {.starts_at = str_start,
                          .row = row,
                          .col = col_start,
                          .len = str_len,
                          tok.kind = TokenType_Ident,
                          tok.contents =
                              (union TokenContents){.arena_pointer = str}};
      for (int i = 0; i < KEYWORD_COUNT; i++) {
        int key_word = KeyWords[i];
        if (strcmp(keywords_from_str(key_word),
                   mod_parser->arena.start + arena_start_offset) == 0) {
          tok.kind = key_word;
          tok.contents = (union TokenContents){.null = NULL};
        }
      }
      token_to_json(mod_parser, stdout, &tok);
      vec_token_push(mod_parser->tokens, tok);
      continue;
    }

    if (cur_char == '"') {
      at++;
      int str_start = at;
      unsigned long row_start = row;
      unsigned long col_start = col;
      int str_len = -1;
      int str_char = 0;
      int arena_start_offset = mod_parser->arena.offset;
      while (str_char != EOF) {
        str_len++;
        str_char = src_code[at];
        if (str_char == EOF || str_char == 0) {
          vec_error_push(mod_parser->errs, (struct Error){
                                               .at = str_start,
                                               .row = row_start,
                                               .col = col_start,
                                               .msg = "Unclosed string",
                                               .context = "lexing",
                                               .len = 1,
                                           });
          break;
        }
        if (str_char == '"') {
          break;
        }
        at++;
        col++;
        if (str_char == '\n') {
          row++;
          col = 0;
        }
        ((char *)(mod_parser->arena.start + mod_parser->arena.offset))[0] =
            str_char;
        mod_parser->arena.offset += sizeof(char);
      }
      struct ArenaPointer str = {
          .offset = arena_start_offset,
          .len = str_len * sizeof(char),
      };
      struct Token tok = {.starts_at = str_start,
                          .row = row,
                          .col = col_start,
                          .len = str_len + 1,
                          .kind = TokenType_Str,
                          .contents = {.arena_pointer = str}};
      token_to_json(mod_parser, stdout, &tok);
      vec_token_push(mod_parser->tokens, tok);
      continue;
    }

    int msg_len = get_string_size("Unhandled char case `%c`\n", cur_char);
    char *msg = malloc(msg_len);
    snprintf(msg, msg_len, "Unhandled char case `%c`\n", cur_char);
    vec_error_push(mod_parser->errs, (struct Error){
                                         .at = at,
                                         .row = row,
                                         .col = col,
                                         .msg = msg,
                                         .context = "lexing",
                                         .len = 1,
                                     });
    json_dumpf(json_pack("{s: s, s: s, s:i}\n", "error", "unhandled_char",
                         "char", &cur_char, "int", cur_char),
               stdout, 0);
    puts("");
  }
  json_dumpf(json_pack("{s: s}", "msg", "finished lexing"), stdout, 0);
  puts("");

  free(src_code);
  return 0;
}

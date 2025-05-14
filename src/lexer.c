#include "lexer.h"
#include "arena.h"
#include "mod.h"
#include <ctype.h>
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
  default:
    return "Unknown";
  }
}
int print_str(struct ModParser *mod_parser, FILE *out_stream,
              struct ArenaPointer *str_ptr) {
  if (str_ptr == NULL) {
    return -1;
  }
  for (int i = 0; i < str_ptr->len; i++) {
    char *char_ptr =
        mod_parser->arena.start + str_ptr->offset + (i * sizeof(char));
    fprintf(out_stream, "%c", *char_ptr);
  }
  return 0;
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
  fprintf(out_stream,
          "{\"starts_at\": %d, \"len\": %d,\"kind\":\"%s\", \"contents\": ",
          token->starts_at, token->len, token_type_to_str(token->kind));
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
    fprintf(out_stream, "null");
    break;
  case TokenType_Int:
    fprintf(out_stream, "%d", token->contents.int_val);
    break;
  case TokenType_Str:
    fprintf(out_stream, "\"");
    print_str(mod_parser, out_stream, &token->contents.arena_pointer);
    fprintf(out_stream, "\"");
    break;
  case TokenType_Ident:
    fprintf(out_stream, "\"");
    print_str(mod_parser, out_stream, &token->contents.arena_pointer);
    fprintf(out_stream, "\"");
    break;
  // TokenType_BracketOpen,
  // TokenType_BracketClose,
  // TokenType_SquareBracketOpen,
  // TokenType_SquareBracketClose,
  // TokenType_CurlyBracketOpen,
  // TokenType_CurlyBracketClose,
  // TokenType_Comma,
  // TokenType_Amp,
  // TokenType_Plus,
  // TokenType_Minus,
  // TokenType_Times,
  // TokenType_Divide,
  // TokenType_Assign,
  // TokenType_Equiv,
  default:
    fprintf(out_stream, "\"UNKNOWN\"");
  }
  fprintf(out_stream, "}\n");
  return 0;
}

struct FileCharIter {
  FILE *in_stream;
  size_t chunk_size;
  char *buffer;
  size_t offset;
  size_t read_count;
};
int file_char_iter_next(struct FileCharIter *file_char_iter) {
  // printf("file_char_iter_next: offset: %zu, read_count %zu, chunk_size: %zu\n",
  //        file_char_iter->offset, file_char_iter->read_count,
  //        file_char_iter->chunk_size);
  if (file_char_iter->offset >= file_char_iter->chunk_size) {
      puts("doing fread");
    file_char_iter->read_count =
        fread(file_char_iter->buffer, file_char_iter->chunk_size, 1,
                       file_char_iter->in_stream);
    file_char_iter->offset = 0;
    if (file_char_iter->read_count == 0) {
        exit(-1);
      return -1;
    }
  }
  //   size_t buffer_read = fread_unlocked(buffer, BUF_READ_SIZE, 1, in_stream);
  int res = file_char_iter->buffer[file_char_iter->offset];
  file_char_iter->offset++;
  return res;
}

#define BUF_READ_SIZE 100
int lex_file(FILE *in_stream, struct ModParser *mod_parser) {
  struct FileCharIter char_iter = {
      .in_stream = in_stream,
      .chunk_size = BUF_READ_SIZE,
      .buffer = malloc(BUF_READ_SIZE),
      .offset = 0,
      .read_count = 0,
  };
  char_iter.read_count = fread(char_iter.buffer, char_iter.chunk_size,
                                        1, char_iter.in_stream);
  int cur_char = 1;
  uint at = -1;
  while (cur_char != '\0') {
    at++;
    cur_char = file_char_iter_next(&char_iter);
    struct Token tok = {.starts_at = at,
                        .len = 1,
                        tok.kind = TokenType_BracketOpen,
                        tok.contents = (union TokenContents){.null = NULL}};
    switch (cur_char) {
    case '\n':
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
      // TODO: This wont handle if the peek is at the end of the buffer
      char next_char = char_iter.buffer[char_iter.offset + 1];
      if (next_char == '=') {
        at++;
        tok.kind = TokenType_Equiv;
      } else {
        tok.kind = TokenType_Assign;
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
      int str_len = 0;
      int str_char = 0;
      int arena_start_offset = mod_parser->arena.offset;

      ((char *)(mod_parser->arena.start + mod_parser->arena.offset))[0] =
          cur_char;
      mod_parser->arena.offset += sizeof(char);

      while (str_char != EOF) {
        str_len++;
        str_char = file_char_iter_next(&char_iter);
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
      struct ArenaPointer str = {
          .offset = arena_start_offset,
          .len = str_len * sizeof(char),
      };
      char *str_ptr = arena_get_pointer(&mod_parser->arena, str.offset);
      int int_val = strtol(str_ptr, NULL, 10);
      struct Token tok = {.starts_at = str_start,
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
      int str_len = 0;
      int str_char = 0;
      int arena_start_offset = mod_parser->arena.offset;

      ((char *)(mod_parser->arena.start + mod_parser->arena.offset))[0] =
          cur_char;
      mod_parser->arena.offset += sizeof(char);

      while (str_char != EOF) {
        str_len++;
        str_char = file_char_iter_next(&char_iter);
        if (!isalpha(str_char) && !isdigit(str_char) && str_char != '_') {
          break;
        }

        ((char *)(mod_parser->arena.start + mod_parser->arena.offset))[0] =
            str_char;
        mod_parser->arena.offset += sizeof(char);
      }
      at += str_len - 1;
      struct ArenaPointer str = {
          .offset = arena_start_offset,
          .len = str_len * sizeof(char),
      };
      struct Token tok = {.starts_at = str_start,
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
      int str_len = -1;
      int str_char = 0;
      int arena_start_offset = mod_parser->arena.offset;
      while (str_char != EOF) {
        str_len++;
        // str_char = buffer[at + str_len];
        str_char = file_char_iter_next(&char_iter);
        if (str_char == '"') {
          break;
        }
        ((char *)(mod_parser->arena.start + mod_parser->arena.offset))[0] =
            str_char;
        mod_parser->arena.offset += sizeof(char);
      }
      at += str_len;
      struct ArenaPointer str = {
          .offset = arena_start_offset,
          .len = str_len * sizeof(char),
      };
      struct Token tok = {.starts_at = str_start,
                          .len = str_len,
                          .kind = TokenType_Str,
                          .contents = {.arena_pointer = str}};
      token_to_json(mod_parser, stdout, &tok);
      vec_token_push(mod_parser->tokens, tok);
      continue;
    }

    printf("{\"error\": \"unhandled_char\", \"char\": \"%c\", \"int\": %d}\n",
           cur_char, cur_char);
  }
  free(char_iter.buffer);
  puts("{\"msg\":\"finished lexing\"}");
  // print_arena_state(mod_parser);
  return 0;
}
// int lex_file(FILE *in_stream, struct ModParser *mod_parser) {
//   int cur_char = 1;
//   uint at = -1;
//   char buffer[BUF_READ_SIZE] = {0};
//   size_t buffer_read = fread_unlocked(buffer, BUF_READ_SIZE, 1, in_stream);
//   while (cur_char != '\0') {
//     at++;
//     cur_char = buffer[at];
//     struct Token tok = {.starts_at = at,
//                         .len = 1,
//                         tok.kind = TokenType_BracketOpen,
//                         tok.contents = (union TokenContents){.null = NULL}};
//     switch (cur_char) {
//     case '\n':
//       continue;
//     case '(':
//       tok.kind = TokenType_BracketOpen;
//       token_to_json(mod_parser, stdout, &tok);
//       vec_token_push(mod_parser->tokens, tok);
//       continue;
//     case ')':
//       tok.kind = TokenType_BracketClose;
//       token_to_json(mod_parser, stdout, &tok);
//       vec_token_push(mod_parser->tokens, tok);
//       continue;
//     case '[':
//       tok.kind = TokenType_SquareBracketOpen;
//       token_to_json(mod_parser, stdout, &tok);
//       vec_token_push(mod_parser->tokens, tok);
//       continue;
//     case ']':
//       tok.kind = TokenType_SquareBracketClose;
//       token_to_json(mod_parser, stdout, &tok);
//       vec_token_push(mod_parser->tokens, tok);
//       continue;
//     case '{':
//       tok.kind = TokenType_CurlyBracketOpen;
//       token_to_json(mod_parser, stdout, &tok);
//       vec_token_push(mod_parser->tokens, tok);
//       continue;
//     case '}':
//       tok.kind = TokenType_CurlyBracketClose;
//       token_to_json(mod_parser, stdout, &tok);
//       vec_token_push(mod_parser->tokens, tok);
//       continue;
//     case ',':
//       tok.kind = TokenType_Comma;
//       token_to_json(mod_parser, stdout, &tok);
//       vec_token_push(mod_parser->tokens, tok);
//       continue;
//     case '.':
//       tok.kind = TokenType_Dot;
//       token_to_json(mod_parser, stdout, &tok);
//       vec_token_push(mod_parser->tokens, tok);
//       continue;
//     case '&':
//       tok.kind = TokenType_Amp;
//       token_to_json(mod_parser, stdout, &tok);
//       vec_token_push(mod_parser->tokens, tok);
//       continue;
//     case '+':
//       tok.kind = TokenType_Plus;
//       token_to_json(mod_parser, stdout, &tok);
//       vec_token_push(mod_parser->tokens, tok);
//       continue;
//     case '-':
//       tok.kind = TokenType_Minus;
//       token_to_json(mod_parser, stdout, &tok);
//       vec_token_push(mod_parser->tokens, tok);
//       continue;
//     case '*':
//       tok.kind = TokenType_Times;
//       token_to_json(mod_parser, stdout, &tok);
//       vec_token_push(mod_parser->tokens, tok);
//       continue;
//     case '/':
//       tok.kind = TokenType_Divide;
//       token_to_json(mod_parser, stdout, &tok);
//       vec_token_push(mod_parser->tokens, tok);
//       continue;
//     case '=': {
//       char next_char = buffer[at + 1];
//       if (next_char == '=') {
//         at++;
//         tok.kind = TokenType_Equiv;
//       } else {
//         tok.kind = TokenType_Assign;
//       }
//       token_to_json(mod_parser, stdout, &tok);
//       vec_token_push(mod_parser->tokens, tok);
//       continue;
//     }
//     }
//     if (cur_char == '\n') {
//
//       continue;
//     } else if (isspace(cur_char)) {
//       continue;
//     }
//     if (isdigit(cur_char)) {
//       int str_start = at;
//       int str_len = 0;
//       int str_char = 0;
//       int arena_start_offset = mod_parser->arena.offset;
//
//       ((char *)(mod_parser->arena.start + mod_parser->arena.offset))[0] =
//           cur_char;
//       mod_parser->arena.offset += sizeof(char);
//
//       while (str_char != EOF) {
//         str_len++;
//         str_char = buffer[at + str_len];
//         if (str_char == '_') {
//           continue;
//         }
//         if (!isdigit(str_char)) {
//           break;
//         }
//
//         ((char *)(mod_parser->arena.start + mod_parser->arena.offset))[0] =
//             str_char;
//         mod_parser->arena.offset += sizeof(char);
//       }
//       at += str_len - 1;
//       struct ArenaPointer str = {
//           .offset = arena_start_offset,
//           .len = str_len * sizeof(char),
//       };
//       char *str_ptr = arena_get_pointer(&mod_parser->arena, str.offset);
//       int int_val = strtol(str_ptr, NULL, 10);
//       struct Token tok = {.starts_at = str_start,
//                           .len = str_len,
//                           tok.kind = TokenType_Int,
//                           tok.contents =
//                               (union TokenContents){.int_val = int_val}};
//       token_to_json(mod_parser, stdout, &tok);
//       vec_token_push(mod_parser->tokens, tok);
//       continue;
//     }
//     if (isalpha(cur_char)) {
//       int str_start = at;
//       int str_len = 0;
//       int str_char = 0;
//       int arena_start_offset = mod_parser->arena.offset;
//
//       ((char *)(mod_parser->arena.start + mod_parser->arena.offset))[0] =
//           cur_char;
//       mod_parser->arena.offset += sizeof(char);
//
//       while (str_char != EOF) {
//         str_len++;
//         str_char = buffer[at + str_len];
//         if (!isalpha(str_char) && !isdigit(str_char) && str_char != '_') {
//           break;
//         }
//
//         ((char *)(mod_parser->arena.start + mod_parser->arena.offset))[0] =
//             str_char;
//         mod_parser->arena.offset += sizeof(char);
//       }
//       at += str_len - 1;
//       struct ArenaPointer str = {
//           .offset = arena_start_offset,
//           .len = str_len * sizeof(char),
//       };
//       struct Token tok = {.starts_at = str_start,
//                           .len = str_len,
//                           tok.kind = TokenType_Ident,
//                           tok.contents =
//                               (union TokenContents){.arena_pointer = str}};
//       for (int i = 0; i < KEYWORD_COUNT; i++) {
//         int key_word = KeyWords[i];
//         if (strcmp(keywords_from_str(key_word),
//                    mod_parser->arena.start + arena_start_offset) == 0) {
//           tok.kind = key_word;
//           tok.contents = (union TokenContents){.null = NULL};
//         }
//       }
//       token_to_json(mod_parser, stdout, &tok);
//       vec_token_push(mod_parser->tokens, tok);
//       continue;
//     }
//
//     if (cur_char == '"') {
//       at++;
//       int str_start = at;
//       int str_len = -1;
//       int str_char = 0;
//       int arena_start_offset = mod_parser->arena.offset;
//       while (str_char != EOF) {
//         str_len++;
//         str_char = buffer[at + str_len];
//         if (str_char == '"') {
//           break;
//         }
//         ((char *)(mod_parser->arena.start + mod_parser->arena.offset))[0] =
//             str_char;
//         mod_parser->arena.offset += sizeof(char);
//       }
//       at += str_len;
//       struct ArenaPointer str = {
//           .offset = arena_start_offset,
//           .len = str_len * sizeof(char),
//       };
//       struct Token tok = {.starts_at = str_start,
//                           .len = str_len,
//                           .kind = TokenType_Str,
//                           .contents = {.arena_pointer = str}};
//       token_to_json(mod_parser, stdout, &tok);
//       vec_token_push(mod_parser->tokens, tok);
//       continue;
//     }
//
//     printf("{\"error\": \"unhandled_char\", \"char\": \"%c\", \"int\":
//     %d}\n",
//            cur_char, cur_char);
//   }
//   puts("{\"msg\":\"finished lexing\"}");
//   // print_arena_state(mod_parser);
//   return 0;
// }

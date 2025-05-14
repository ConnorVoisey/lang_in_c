#ifndef FILE_lexer_h
#define FILE_lexer_h

#include "arena.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

enum TokenType {
  TokenType_ImportKeyWord,
  TokenType_ExternKeyWord,
  TokenType_ReturnKeyWord,
  TokenType_LetKeyWord,
  TokenType_StructKeyWord,
  TokenType_PubKeyWord,
  TokenType_FnKeyWord,
  TokenType_HeadersKeyWord,
  TokenType_DefsKeyWord,
  TokenType_Int,
  TokenType_Str,
  TokenType_Ident,
  TokenType_BracketOpen,
  TokenType_BracketClose,
  TokenType_SquareBracketOpen,
  TokenType_SquareBracketClose,
  TokenType_CurlyBracketOpen,
  TokenType_CurlyBracketClose,
  TokenType_Comma,
  TokenType_Dot,
  TokenType_Amp,
  TokenType_Plus,
  TokenType_Minus,
  TokenType_Times,
  TokenType_Divide,
  TokenType_Assign,
  TokenType_Equiv,
};
union TokenContents {
  char *str;
  int int_val;
  struct ArenaPointer arena_pointer;
  void *null;
};

struct Token {
  uint starts_at;
  uint len;
  int kind;
  union TokenContents contents;
};

struct ModParser;
int lex_file(FILE *in_stream, struct ModParser *mod_parser);

#endif

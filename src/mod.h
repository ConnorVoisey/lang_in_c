#ifndef FILE_mod_h
#define FILE_mod_h

#include "arena.h"
#include "lexer.h"
#include "vec.h"

struct ModParser {
  struct Arena arena;
  struct VecToken* tokens;
};

#endif

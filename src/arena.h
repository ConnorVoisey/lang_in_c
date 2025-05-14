#ifndef FILE_arena_h
#define FILE_arena_h

#include <stdlib.h>
struct Arena {
  void *start;
  int capacity;
  int offset;
};

void arena_clear(struct Arena *arena);

struct Arena *arena_create(int capacity);

void arena_increase_capacity(struct Arena *arena, int new_capacity);

void *arena_get_pointer(struct Arena *arena, int offset);

struct ArenaPointer {
  int offset;
  int len;
};

#endif

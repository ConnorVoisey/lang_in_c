#include "arena.h"
#include <stdlib.h>

void arena_clear(struct Arena *arena) { arena->offset = 0; }

struct Arena *arena_create(int capacity) {
  struct Arena *arena = malloc(sizeof(struct Arena));
  arena->capacity = capacity;
  arena->start = malloc(capacity);
  return arena;
}

void arena_increase_capacity(struct Arena *arena, int new_capacity) {
  arena->capacity = new_capacity;
  arena->start = realloc(arena->start, new_capacity);
}

void *arena_get_pointer(struct Arena *arena, int offset) {
  return arena->start + offset;
}

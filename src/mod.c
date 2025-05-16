#include "mod.h"
#include <stdarg.h>

int get_string_size(const char *fmt, ...) {
  va_list args;
  int len;

  va_start(args, fmt);
  len = vsnprintf(NULL, 0, fmt, args);
  va_end(args);

  return len;
}

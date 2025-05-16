#ifndef FILE_error_h
#define FILE_error_h

struct Error {
  unsigned long at;
  unsigned int row;
  unsigned int col;
  unsigned int len;
  char *msg;
  char *context;
};
struct VecError {
  struct Error *data;
  int capacity;
  int length;
};

struct VecError *vec_error_init(int capacity);
void vec_error_push(struct VecError *vec, struct Error err);

#endif

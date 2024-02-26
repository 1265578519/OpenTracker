#include "buffer.h"

void buffer_seek(buffer* b,size_t len) {
  size_t n=b->p+len;
  if (n<b->p) n=b->p;
  if (n>b->n) n=b->n;
  b->p=n;
}

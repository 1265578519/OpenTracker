#include "byte.h"
#include "buffer.h"

ssize_t buffer_peekc(buffer* b,char* x) {
  if (b->p==b->n) {
    register ssize_t blen;
    if ((blen=buffer_feed(b))<=0) return blen;
  }
  *x=b->x[b->p];
  return 1;
}

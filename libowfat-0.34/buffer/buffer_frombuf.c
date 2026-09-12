#include "stralloc.h"
#include "buffer.h"

static ssize_t dummyreadwrite(int fd,char* buf,size_t len) {
  (void)fd;
  (void)buf;
  (void)len;
  return 0;
}

void buffer_frombuf(buffer* b,const char* x,size_t l) {
  b->x=(char*)x;
  b->p=0;
  b->n=l;
  b->a=l;
  b->fd=0;
  b->op=dummyreadwrite;
  b->deinit=0;
}

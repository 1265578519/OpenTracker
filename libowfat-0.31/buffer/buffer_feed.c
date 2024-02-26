#include "buffer.h"

extern ssize_t buffer_stubborn_read(ssize_t (*op)(),int fd,const char* buf, size_t len,void* cookie);

ssize_t buffer_feed(buffer* b) {
  if (b->p==b->n) {
    ssize_t w;
    if ((w=buffer_stubborn_read(b->op,b->fd,b->x,b->a,b))<0)
      return -1;
    b->n=(size_t)w;
    b->p=0;
  }
  return (ssize_t)(b->n-b->p);
}

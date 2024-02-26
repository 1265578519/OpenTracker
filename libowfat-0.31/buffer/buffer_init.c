#include "buffer.h"

void buffer_init(buffer* b,ssize_t (*op)(),int fd,
		 char* y,size_t ylen) {
  b->op=op;
  b->fd=fd;
  b->x=y;
  b->a=ylen;
  b->p=0;
  b->n=0;
  b->cookie=0;
  b->deinit=0;
}

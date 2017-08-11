#include "buffer.h"

void buffer_init_free(buffer* b,ssize_t (*op)(),int fd,
		 char* y,size_t ylen) {
  buffer_init(b,op,fd,y,ylen);
  b->deinit=buffer_free;
}

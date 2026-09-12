#include <stdlib.h>
#include <unistd.h>
#include "buffer.h"

int buffer_init_allocbuf(buffer* b, ssize_t (*op)(), int fd, size_t ylen) {
  if (fd==-1) return -1;
  char* thebuffer;
  if (!(thebuffer=malloc(ylen)))
    return -1;
  buffer_init_free(b, op, fd, thebuffer, ylen);
  return 0;
}


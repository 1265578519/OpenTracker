#include <unistd.h>
#include "buffer.h"

int buffer_init_write_allocbuf(buffer* b, int fd, size_t ylen) {
  return buffer_init_allocbuf(b, write, fd, ylen);
}


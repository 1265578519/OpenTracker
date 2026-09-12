#include <unistd.h>
#include <buffer.h>

int buffer_init_write(buffer* b, int fd, char* y,size_t ylen) {
  if (fd==-1) return -1;
  buffer_init(b, write, fd, y, ylen);
  return 0;
}


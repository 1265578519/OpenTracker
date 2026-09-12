#include <unistd.h>
#include <buffer.h>

int buffer_init_read(buffer* b, int fd, char* y,size_t ylen) {
  if (fd==-1) return -1;
  buffer_init(b, read, fd, y, ylen);
  return 0;
}


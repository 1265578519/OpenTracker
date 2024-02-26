#include <unistd.h>
#include "buffer.h"

static ssize_t b0read(int fd,char* buf, size_t len) {
  if (buffer_flush(buffer_1)<0) return -1;
  return read(fd,buf,len);
}

char buffer_0_space[BUFFER_INSIZE];
static buffer it = BUFFER_INIT_READ(b0read,0,buffer_0_space,sizeof buffer_0_space);
buffer *buffer_0 = &it;


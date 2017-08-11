#include <errno.h>
#include "buffer.h"

ssize_t buffer_stubborn_read(ssize_t (*op)(),int fd,const char* buf, size_t len,void* cookie) {
  ssize_t w;
  for (;;) {
    if ((w=op(fd,buf,len,cookie))<0)
      if (errno == EINTR) continue;
    break;
  }
  return w;
}


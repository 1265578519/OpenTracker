#include <errno.h>
#include "buffer.h"

ssize_t buffer_stubborn(ssize_t (*op)(),int fd,const char* buf, size_t len,void* cookie) {
  ssize_t w;
  errno=0;
  while (len) {
    if ((w=op(fd,buf,len,cookie))<=0) {
      if (errno == EINTR) continue;
      return -1;
    }
    buf+=w;
    len-=(size_t)w;
  }
  return 0;
}

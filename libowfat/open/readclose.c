#include <unistd.h>
#include <errno.h>
#include "readclose.h"

ssize_t readclose_append(int fd,stralloc *sa,size_t bufsize)
{
  ssize_t r;
  for (;;) {
    if (!stralloc_readyplus(sa,bufsize)) { close(fd); return -1; }
    r = read(fd,sa->s + sa->len,bufsize);
    if (r == -1) if (errno == EINTR) continue;
    if (r <= 0) { close(fd); return r; }
    sa->len += (size_t)r;
  }
}

ssize_t readclose(int fd,stralloc *sa,size_t bufsize)
{
  if (!stralloc_copys(sa,"")) { close(fd); return -1; }
  return readclose_append(fd,sa,bufsize);
}

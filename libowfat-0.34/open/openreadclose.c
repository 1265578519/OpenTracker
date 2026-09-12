#include <errno.h>
#include "open.h"
#include "readclose.h"
#include "openreadclose.h"

int openreadclose(const char *fn,stralloc *sa,size_t bufsize)
{
  int fd;
  fd = open_read(fn);
  if (fd == -1) {
    if (errno == ENOENT) return 0;
    return -1;
  }
  if (readclose(fd,sa,bufsize) == -1) return -1;
  return 1;
}

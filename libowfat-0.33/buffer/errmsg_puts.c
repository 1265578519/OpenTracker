#include "errmsg.h"
#include "errmsg_int.h"
#include "str.h"

#ifdef __MINGW32__

void errmsg_puts(int fd,const char* s) {
  return write(fd,s,str_len(s));
}

void errmsg_flush(int fd) {
  return 0;
}

#else
#include <sys/uio.h>

enum { COUNT=25 };
static struct iovec x[COUNT];
static int l;

void errmsg_puts(int fd,const char* s) {
  x[l].iov_base=(char*)s;
  x[l].iov_len=str_len(s);
  if (++l==COUNT) errmsg_flush(fd);
}

void errmsg_flush(int fd) {
  int n=l;
  l=0;
  if (n) writev(fd,x,n);
}
#endif

void errmsg_start(int fd) {
  if (argv0) {
    errmsg_puts(fd,argv0);
    errmsg_puts(fd,": ");
  }
}


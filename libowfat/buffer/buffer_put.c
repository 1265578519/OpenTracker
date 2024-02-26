#include <string.h>
#include <unistd.h>
#ifndef __MINGW32__
#include <sys/uio.h>
#endif
#include "buffer.h"

extern int buffer_stubborn(ssize_t (*op)(),int fd,const char* buf, size_t len,void* cookie);

#ifndef __unlikely
#ifdef __GNUC__
#define __unlikely(x) __builtin_expect((x),0)
#else
#define __unlikely(x) (x)
#endif
#endif

int buffer_put(buffer* b,const char* buf,size_t len) {
  if (__unlikely(len>b->a-b->p)) {	/* doesn't fit */
#ifndef __MINGW32__
    if (b->op==write) {
      /* if it's write, we can substitute writev */
      struct iovec v[2];
      ssize_t r;
      v[0].iov_base=b->x; v[0].iov_len=b->p;
      v[1].iov_base=(char*)buf; v[1].iov_len=len;
      r=writev(b->fd,v,2);
      if (r<0) return -1;
      if ((size_t)r>=b->p) {
	r-=b->p;
	b->p=0;
	buf+=r;
	len-=r;
	if (len) goto do_memcpy;
	return 0;
      } /* else fall through */
    }
#endif
    if (buffer_flush(b)==-1) return -1;
    if (len>b->a) {
      if (buffer_stubborn(b->op,b->fd,buf,len,b)<0) return -1;
      return 0;
    }
  }
do_memcpy:
  memcpy(b->x+b->p, buf, len);
  b->p+=len;
  return 0;
}

#include "byte.h"
#include "buffer.h"

ssize_t buffer_get(buffer* b,char* x,size_t len) {
  ssize_t done;
  ssize_t blen;
  done=0;
  if ((ssize_t)len < 0) len=(ssize_t)(((size_t)-1)>>1);
  while (len) {
    if ((blen=buffer_feed(b))<0) return blen;
    if (blen==0) break;
    if (blen>=(ssize_t)len)
      blen=(ssize_t)len;
    byte_copy(x,(size_t)blen,b->x+b->p);
    b->p+=(size_t)blen;
    len-=(size_t)blen;
    x+=blen;
    done+=blen;
  }
  return done;
}

#include "byte.h"
#include "buffer.h"
#include "scan.h"

#include <string.h>	/* for memccpy */

ssize_t buffer_get_token(buffer* b,char* x,size_t len,const char* charset,size_t setlen) {
  size_t blen;

  if ((ssize_t)len<0) len=(ssize_t)(((size_t)-1)>>1);
  if (setlen==1) {
    for (blen=0; blen<len; ) {
      ssize_t n=buffer_feed(b);
      char* d;
      if (n<=0) return blen;
      if (n>(ssize_t)(len-blen)) n=len-blen;
      if ((d=memccpy(x+blen,b->x+b->p,(unsigned char)charset[0],n))) {
	/* memccpy returns a pointer to the next char after matching
	 * char or NULL if it copied all bytes it was asked for */
	b->p+=(d-(x+blen));
	return d-x-1;
      }
      blen+=n;
      b->p+=n;
    }
  } else {
    for (blen=0;blen<len;++blen) {
      register ssize_t r;
      if ((r=buffer_getc(b,x))<0) return r;
      if (r==0) { *x=0; break; }
      if (byte_chr(charset,setlen,*x)<setlen) break;
      ++x;
    }
  }
  return (ssize_t)blen;
}

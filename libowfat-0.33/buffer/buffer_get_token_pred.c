#include "byte.h"
#include "buffer.h"
#include "scan.h"

ssize_t buffer_get_token_pred(buffer* b,char* x,size_t len,
			  string_predicate p) {
  unsigned int blen;

  for (blen=0;blen<len;++blen) {
    register ssize_t r;
    if ((r=buffer_getc(b,x))<0) return r;
    if (r==0) break;
    if (p(x-blen,blen+1)) break;
    ++x;
  }
  return blen;
}

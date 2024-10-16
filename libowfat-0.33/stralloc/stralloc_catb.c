#include "byte.h"
#include "stralloc.h"

/* stralloc_catb adds the string buf[0], buf[1], ... buf[len-1] to the
 * end of the string stored in sa, allocating space if necessary, and
 * returns 1. If sa is unallocated, stralloc_catb is the same as
 * stralloc_copyb. If it runs out of memory, stralloc_catb leaves sa
 * alone and returns 0. */
int stralloc_catb(stralloc *sa,const char *buf,size_t len) {
  if (stralloc_readyplus(sa,len)) {
    byte_copy(sa->s+sa->len,len,buf);
    sa->len+=len;
    return 1;
  }
  return 0;
}

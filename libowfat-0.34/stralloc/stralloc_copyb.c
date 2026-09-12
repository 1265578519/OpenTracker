#include "byte.h"
#include "stralloc.h"

/* stralloc_copyb copies the string buf[0], buf[1], ..., buf[len-1] into
 * sa, allocating space if necessary, and returns 1. If it runs out of
 * memory, stralloc_copyb leaves sa alone and returns 0. */
int stralloc_copyb(stralloc *sa,const char *buf,size_t len) {
  if (stralloc_ready(sa,len)) {
    sa->len=len;
    byte_copy(sa->s,len,buf);
    return 1;
  }
  return 0;
}

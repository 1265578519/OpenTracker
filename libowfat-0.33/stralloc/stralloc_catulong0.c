#include "stralloc.h"
#include "fmt.h"

int stralloc_catulong0(stralloc *sa,unsigned long int in,size_t n) {
  if (stralloc_readyplus(sa,fmt_ulong0(0,in,n))) {
    sa->len+=fmt_ulong0(sa->s+sa->len,in,n);
    return 1;
  } else
    return 0;
}

#include "stralloc.h"
#include "fmt.h"

int stralloc_catlong0(stralloc *sa,signed long int in,size_t n) {
  int neg=-(in<0);
  if (neg) in=-in;
  if (stralloc_readyplus(sa,fmt_minus(0,neg)+fmt_ulong0(0,(unsigned long)in,n))) {
    sa->len+=fmt_minus(sa->s+sa->len,neg);
    sa->len+=fmt_ulong0(sa->s+sa->len,(unsigned long)in,n);
    return 1;
  } else
    return 0;
}

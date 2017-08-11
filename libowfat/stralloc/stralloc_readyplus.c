#include "stralloc.h"
#include <stdlib.h>

/* stralloc_readyplus is like stralloc_ready except that, if sa is
 * already allocated, stralloc_readyplus adds the current length of sa
 * to len. */
int stralloc_readyplus(stralloc *sa,size_t len) {
  if (sa->s) {
    if (sa->len + len < len) return 0;	/* catch integer overflow */
    return stralloc_ready(sa,sa->len+len);
  } else
    return stralloc_ready(sa,len);
}

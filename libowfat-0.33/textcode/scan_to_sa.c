#include "str.h"
#include "stralloc.h"
#include "textcode.h"

size_t scan_to_sa(size_t (*func)(const char*,char*,size_t*),
			 const char* src,stralloc* sa) {
  size_t written;
  size_t r;
  if (!stralloc_readyplus(sa,str_len(src))) return 0;
  if ((r=func(src,sa->s+sa->len,&written)))
    sa->len+=written;
  return r;
}

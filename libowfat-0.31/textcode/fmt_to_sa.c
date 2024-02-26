#include "stralloc.h"
#include "textcode.h"

int fmt_to_sa(size_t (*func)(char*,const char*,size_t),
	      stralloc* sa,const char* src,size_t len) {
  size_t needed=func(0,src,len);
  if (!stralloc_readyplus(sa,needed)) return 0;
  func(sa->s+sa->len,src,len);
  sa->len+=needed;
  return needed;
}

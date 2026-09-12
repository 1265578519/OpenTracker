#include "stralloc.h"
#include "byte.h"
#include "str.h"

extern int stralloc_diffs(const stralloc* a,const char* b) {
  register size_t i;
  register int j;
  for (i=0;;++i) {
    if (i==a->len) return (!b[i])?0:-1;
    if (!b[i]) return 1;
    if ((j=((unsigned char)(a->s[i])-(unsigned char)(b[i])))) return j;
  }
  return j;
}


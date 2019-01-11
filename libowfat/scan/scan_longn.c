#include "scan.h"

static const unsigned long maxlong = ((unsigned long)-1)>>1;

size_t scan_longn(const char *src,size_t n,long *dest) {
  register const char *tmp;
  register long int l;
  register unsigned char c;
  unsigned int neg;
  int ok;
  if (!n--) return 0;
  tmp=src; l=0; ok=0; neg=0;
  switch (*tmp) {
  case '-': neg=1;
  case '+': ++tmp;
  }
  while (n-->0 && (c=(unsigned char)(*tmp-'0'))<10) {
    unsigned long int n;
    /* we want to do: l=l*10+c
     * but we need to check for integer overflow.
     * to check whether l*10 overflows, we could do
     *   if ((l*10)/10 != l)
     * however, multiplication and division are expensive.
     * so instead of *10 we do (l<<3) (i.e. *8) + (l<<1) (i.e. *2)
     * and check for overflow on all the intermediate steps */
    n=(unsigned long)l<<3; if ((n>>3)!=(unsigned long)l) break;
    if (n+((unsigned long)l<<1) < n) break;
    n+=(unsigned long)l<<1;
    if (n+c < n) break;
    n+=c;
    if (n > maxlong+neg) break;
    l=(long)n;
    ++tmp;
    ok=1;
  }
  if (!ok) return 0;
  *dest=(neg?-l:l);
  return (size_t)(tmp-src);
}

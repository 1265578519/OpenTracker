#include "scan.h"

static const unsigned short maxshort = ((unsigned short)-1)>>1;

size_t scan_short(const char* src,short* dest) {
  register const char *tmp;
  register short l;
  register unsigned char c;
  int neg;
  int ok;
  tmp=src; l=0; ok=neg=0;
  switch (*tmp) {
  case '-': neg=1; /* fall through */
  case '+': ++tmp;
  }
  while ((c=(unsigned char)(*tmp-'0'))<10) {
    unsigned short int n;
#if defined(__GNUC__) && (__GNUC__ >= 5)
    if (__builtin_mul_overflow(l,10,&n) || __builtin_add_overflow(n,c,&n))
      break;
#else
    /* we want to do: l=l*10+c
     * but we need to check for integer overflow.
     * to check whether l*10 overflows, we could do
     *   if ((l*10)/10 != l)
     * however, multiplication and division are expensive.
     * so instead of *10 we do (l<<3) (i.e. *8) + (l<<1) (i.e. *2)
     * and check for overflow on all the intermediate steps */
    n=(unsigned short)(l<<3); if ((n>>3)!=(unsigned short)l) break;
    if (n+(l<<1) < n) break;
    n=(unsigned short)(n+(l<<1));
    if (n+c < n) break;
    n=(unsigned short)(n+c);
#endif
    if (n > maxshort+neg) break;
    l=(short)n;
    ++tmp;
    ok=1;
  }
  if (!ok) return 0;
  *dest=(short)(neg?-l:l);
  return (size_t)(tmp-src);
}

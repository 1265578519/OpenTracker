#include "scan.h"

static const unsigned int maxint = ((unsigned int)-1)>>1;

size_t scan_int(const char* src,int* dest) {
  register const char *tmp;
  register int l;
  register unsigned char c;
  unsigned int neg;
  int ok;
  tmp=src; l=0; ok=0; neg=0;
  switch (*tmp) {
  case '-': neg=1; /* fall through */
  case '+': ++tmp;
  }
  while ((c=(unsigned char)(*tmp-'0'))<10) {
    unsigned int n;
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
    n=(unsigned int)l<<3; if ((n>>3)!=(unsigned int)l) break;
    if (n+((unsigned int)l<<1) < n) break;
    n+=(unsigned int)l<<1;
    if (n+c < n) break;
    n+=c;
#endif
    if (n > maxint+neg) break;
    l=(int)n;
    ++tmp;
    ok=1;
  }
  if (!ok) return 0;
  *dest=(neg?-l:l);
  return (size_t)(tmp-src);
}

#ifdef UNITTEST
#include <assert.h>

int main() {
  int i;
  assert(scan_int("1234",&i)==4 && i==1234);
  assert(scan_int("-1234",&i)==5 && i==-1234);
  assert(scan_int("+1234",&i)==5 && i==1234);
  assert(scan_int("4294967295",&i)==9 && i==429496729);	// overflow
  assert(scan_int("2147483647",&i)==10 && i==2147483647);	// MAX_INT
  assert(scan_int("2147483648",&i)==9 && i==214748364);	// overflow
  assert(scan_int("-2147483648",&i)==11 && i==-2147483648);	// MIN_INT
  assert(scan_int("-2147483649",&i)==10 && i==-214748364);	// underflow
  return 0;
}
#endif

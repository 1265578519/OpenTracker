#include "scan.h"

size_t scan_ulonglong(const char *src,unsigned long long *dest) {
  register const char *tmp=src;
  register unsigned long long l=0;
  register unsigned char c;
  while ((c=(unsigned char)(*tmp-'0'))<10) {
    unsigned long long n;
#if defined(__GNUC__) && (__GNUC__ >= 5)
    if (__builtin_mul_overflow(l,10,&n) || __builtin_add_overflow(n,c,&n))
      break;
    l=n;
#else
    /* division is very slow on most architectures */
    n=l<<3; if ((n>>3)!=l) break;
    if (n+(l<<1) < n) break;
    n+=l<<1;
    if (n+c < n) break;
    l=n+c;
#endif
    ++tmp;
  }
  if (tmp-src) *dest=l;
  return (size_t)(tmp-src);
}

#ifdef UNITTEST
#include <assert.h>

int main() {
  unsigned long long l;
  assert(scan_ulonglong("0",&l)==1 && l==0);
  assert(scan_ulonglong("1",&l)==1 && l==1);
  assert(scan_ulonglong("4294967295",&l) == 10 && l==4294967295ull);
  assert(scan_ulonglong("18446744073709551615",&l) == 20 && l==18446744073709551615ull);
  assert(scan_ulonglong("18446744073709551616",&l) == 19 && l==1844674407370955161ull);
  return 0;
}
#endif

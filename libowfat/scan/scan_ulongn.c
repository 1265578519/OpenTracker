#include "scan.h"
#include "haveuint128.h"

size_t scan_ulongn(const char* src,size_t n,unsigned long int* dest) {
  register const char *tmp=src;
  register unsigned long int l=0;
  register unsigned char c;
  /* Since the conditions can be computed at compile time, the compiler
   * should only emit code for one of the implementations, depending on
   * which architecture the code is compiled for. */
#if defined(__GNUC__) && (__GNUC__ >= 5)
  /* implementation for recent gcc or clang */
  int ok=0;
  for (; n-->0 && (c=(unsigned char)(*tmp-'0'))<10; ++tmp) {
    unsigned long v;
    if (__builtin_mul_overflow(l,10,&v) || __builtin_add_overflow(v,c,&v))
      break;
    l=v;
    ok=1;
  }
  if (!ok) return 0;

  *dest=l;
  return (size_t)(tmp-src);
#else
#ifdef HAVE_UINT128
  if (sizeof(unsigned long)==sizeof(unsigned long long) && sizeof(unsigned long)<sizeof(__uint128_t)) {
    /* implementation for 64-bit platforms with gcc */
    for (; n-->0 && (c=(unsigned char)(*tmp-'0'))<10; ++tmp) {
      __uint128_t L=(__uint128_t)l*10+c;
      if ((L >> ((sizeof(L)-sizeof(l))*8))) break;
      l=(unsigned long)L;
    }
    *dest=l;
    return (size_t)(tmp-src);
  } else
#endif
  if (sizeof(unsigned long)<sizeof(unsigned long long)) {
    /* implementation for 32-bit platforms */
    for (; n-->0 && (c=(unsigned char)(*tmp-'0'))<10; ++tmp) {
      unsigned long long  L=(unsigned long long)l*10+c;
      if ((unsigned long)L != L) break;
      l=(unsigned long)L;
    }
    *dest=l;
    return (size_t)(tmp-src);
  } else {
    /* implementation for 64-bit platforms without gcc */
    while (n-->0 && (c=(unsigned char)(*tmp-'0'))<10) {
      unsigned long int n;
      /* we want to do: l=l*10+c
      * but we need to check for integer overflow.
      * to check whether l*10 overflows, we could do
      *   if ((l*10)/10 != l)
      * however, multiplication and division are expensive.
      * so instead of *10 we do (l<<3) (i.e. *8) + (l<<1) (i.e. *2)
      * and check for overflow on all the intermediate steps */
      n=l<<3; if ((n>>3)!=l) break;
      if (n+(l<<1)+c < n) break;
      l=n+(l<<1)+c;
      ++tmp;
    }
    if (tmp-src) *dest=l;
    return (size_t)(tmp-src);
  }
#endif
}

#ifdef UNITTEST
#include <assert.h>

int main() {
  unsigned long l;
  assert(scan_ulongn("4294967295",10,&l) == 10 && l==4294967295ul);
  if (sizeof(unsigned long)==4) {
    assert(scan_ulongn("4294967296",10,&l) == 9 && l==429496729);
  } else {
    assert(scan_ulongn("18446744073709551615",20,&l) == 20 && l==18446744073709551615ull);
    assert(scan_ulongn("18446744073709551616",20,&l) == 19 && l==1844674407370955161ull);
  }
  assert(scan_ulongn("1234",3,&l)==3 && l==123);
  return 0;
}
#endif

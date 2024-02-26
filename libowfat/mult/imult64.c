#if defined(__GNUC__) && (__GNUC__ >= 5)

#include "uint64.h"

int imult64( int64 a, int64 b, int64* c) { return !__builtin_mul_overflow(a,b,c); }

#else

#include "safemult.h"

#if defined(__GNUC__) && (defined(__x86_64__) || defined(__ia64__) || defined(__powerpc64__) || defined(__alpha__) || defined(__mips64__) || defined(__sparc64__))

int imult64(int64 a,int64 b,int64* c) {
  __int128_t x=((__int128_t)a)*b;
  if ((*c=(int64)x) != x) return 0;
  return 1;
}

#else

int imult64(int64 a,int64 b,int64* c) {
  int neg=(a<0);
  uint64 d;
  if (neg) a=-a;
  if (b<0) { neg^=1; b=-b; }
  if (!umult64(a,b,&d)) return 0;
  if (d > 0x7fffffffffffffffULL + neg) return 0;
  *c=(neg?-d:d);
  return 1;
}

#endif

#endif

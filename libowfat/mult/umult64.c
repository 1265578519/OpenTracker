#if defined(__GNUC__) && (__GNUC__ >= 5)

#include "uint64.h"

int umult64(uint64 a,uint64 b,uint64* c) { return !__builtin_mul_overflow(a,b,c); }

#else

#include "haveuint128.h"

#include "safemult.h"

#if defined(HAVE_UINT128)

int umult64(uint64 a,uint64 b,uint64* c) {
  __uint128_t x=((__uint128_t)a)*b;
  if ((*c=(uint64)x) != x) return 0;
  return 1;
}

#else

/* return 1 for overflow, 0 for ok */
int umult64(uint64 a,uint64 b,uint64* c) {
  uint32 ahi=a>>32;
  uint32 alo=(a&0xffffffff);
  uint32 bhi=b>>32;
  uint32 blo=(b&0xffffffff);

  // a=ahi*x+alo, b=bhi*x+blo
  // a*b = (ahi*x+alo) * (bhi*x+blo)
  //     = ahi*x*bhi*x + ahi*x*blo + alo*bhi*x + alo*blo

  // -> overflow if ahi*bhi != zero */
  if (ahi && bhi) return 0;

  a=(uint64)(ahi)*blo+(uint64)(alo)*bhi;
  if (a>0xffffffff) return 0;
  {
    uint64 x=(uint64)(alo)*blo;
    if (x+(a<<32) < x) return 0;
    *c=x+(a<<32);
  }
  return 1;
}

#endif

#endif

#ifdef UNITTEST
#include <assert.h>

int main() {
  uint64 a;
  assert(umult64(4,0x8000000000000000ull,&a)==0);
  assert(umult64(16,0x4500000000000000ull,&a)==0);		// make sure we don't fall for "if a*b<a && a*b<b"
  assert(umult64(5,10,&a)==1 && a==50);
  assert(umult64(0x60000000,0x10000000,&a)==1 && a==0x600000000000000);
  return 0;
}
#endif

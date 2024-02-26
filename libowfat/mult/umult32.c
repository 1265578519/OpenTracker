#if defined(__GNUC__) && (__GNUC__ >= 5)

#include "uint32.h"

int umult32(uint32 a,uint32 b,uint32* c) { return !__builtin_mul_overflow(a,b,c); }

#else

#include "safemult.h"

int umult32(uint32 a,uint32 b,uint32* c) {
  unsigned long long x=(unsigned long long)a*b;
  if (x>0xffffffff) return 0;
  *c=x&0xffffffff;
  return 1;
}

#endif

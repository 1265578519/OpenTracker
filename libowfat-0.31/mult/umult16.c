#if defined(__GNUC__) && (__GNUC__ >= 5)

#include "uint16.h"

int umult16(uint16 a,uint16 b,uint16* c) { return !__builtin_mul_overflow(a,b,c); }

#else

#include "safemult.h"

int umult16(uint16 a,uint16 b,uint16* c) {
  unsigned long x=(unsigned long)a*b;
  if (x>0xffff) return 0;
  *c=x&0xffff;
  return 1;
}

#endif

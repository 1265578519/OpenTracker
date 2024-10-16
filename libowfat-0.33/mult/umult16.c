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


#ifdef UNITTEST
#include <assert.h>

int main() {
  uint16 a;
  assert(umult16(7,10000,&a)==0);
  assert(umult16(5,10,&a)==1 && a==50);
  assert(umult16(6,10000,&a)==1 && a==60000);
  return 0;
}
#endif

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

#ifdef UNITTEST
#include <assert.h>

int main() {
  uint32 a;
  assert(umult32(4,0x80000000,&a)==0);
  assert(umult32(16,0x45000000,&a)==0);		// make sure we don't fall for "if a*b<a && a*b<b"
  assert(umult32(5,10,&a)==1 && a==50);
  assert(umult32(6,0x10000000,&a)==1 && a==0x60000000);
  return 0;
}
#endif

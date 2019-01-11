#if defined(__GNUC__) && (__GNUC__ >= 5)

#include "uint32.h"

int imult32( int32 a, int32 b, int32* c) { return !__builtin_mul_overflow(a,b,c); }

#else

#include "safemult.h"

int imult32(int32 a,int32 b,int32* c) {
  int64 x=(int64)a*b;
  if ((int32)x != x) return 0;
  *c=x;
  return 1;
}

#endif

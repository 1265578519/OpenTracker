#if defined(__GNUC__) && (__GNUC__ >= 5)

#include "uint16.h"

int imult16( int16 a, int16 b, int16* c) { return !__builtin_mul_overflow(a,b,c); }

#else

#include "safemult.h"

int imult16(int16 a,int16 b,int16* c) {
  int32 x=(int32)a*b;
  if ((int16)x != x) return 0;
  *c=x;
  return 1;
}

#endif

#include "tai.h"

void tai_pack(char *s,const struct tai *t)
{
  uint64 x;

  x = t->x;
  s[7] = (char)x; x >>= 8;
  s[6] = (char)x; x >>= 8;
  s[5] = (char)x; x >>= 8;
  s[4] = (char)x; x >>= 8;
  s[3] = (char)x; x >>= 8;
  s[2] = (char)x; x >>= 8;
  s[1] = (char)x; x >>= 8;
  s[0] = (char)x;
}

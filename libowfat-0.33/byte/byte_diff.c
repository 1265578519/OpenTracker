#include "byte.h"

/* byte_diff returns negative, 0, or positive, depending on whether the
 * string one[0], one[1], ..., one[len-1] is lexicographically smaller
 * than, equal to, or greater than the string one[0], one[1], ...,
 * one[len-1]. When the strings are different, byte_diff does not read
 * bytes past the first difference. */
int byte_diff(const void* a, size_t len, const void* b) {
#if 0
  /* this gets miscompiled by gcc 4.3.2 on x86_64 */
  register const unsigned char* s=a;
  register const unsigned char* t=b;
  register const unsigned char* u=t+len;
  register int j;
  j=0;
  for (;;) {
    if (t==u) break; if ((j=((unsigned int)*s-*t))) break; ++s; ++t;
    if (t==u) break; if ((j=((unsigned int)*s-*t))) break; ++s; ++t;
    if (t==u) break; if ((j=((unsigned int)*s-*t))) break; ++s; ++t;
    if (t==u) break; if ((j=((unsigned int)*s-*t))) break; ++s; ++t;
  }
  return j;
#else
  size_t i;
  for (i=0; i<len; ++i) {
    int r=((unsigned char*)a)[i] - ((unsigned char*)b)[i];
    if (r) return r;
  }
  return 0;
#endif
}

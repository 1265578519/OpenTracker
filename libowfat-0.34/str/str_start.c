#include "str.h"

/* str_start returns 1 if the b is a prefix of a, 0 otherwise */
int str_start(const char* a, const char* b) {
#if 1
  size_t i;
  for (i=0; ; ++i) {
    if (!b[i]) return 1;
    if (a[i]!=b[i]) break;
  }
  return 0;
#else
  register const char* s=a;
  register const char* t=b;
  for (;;) {
    if (!*t) return 1;
                       if (*s!=*t) break;
                                          ++s; ++t;
    if (!*t) return 1;
                       if (*s!=*t) break;
                                          ++s; ++t;
    if (!*t) return 1;
                       if (*s!=*t) break;
                                          ++s; ++t;
    if (!*t) return 1;
                       if (*s!=*t) break;
                                          ++s; ++t;
  }
  return 0;
#endif
}

#ifdef UNITTEST
#include <assert.h>
int main() {
  assert(str_start("fnord","no")==0);
  assert(str_start("fnord","fno")==1);
  assert(str_start("fnord","fnord")==1);
  assert(str_start("fnord","fnord1")==0);
  return 0;
}
#endif

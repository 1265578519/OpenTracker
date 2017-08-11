#include "str.h"

/* str_start returns 1 if the b is a prefix of a, 0 otherwise */
int str_start(const char* a, const char* b) {
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
}

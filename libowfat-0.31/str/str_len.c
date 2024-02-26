#undef __dietlibc__
#include "str.h"

size_t str_len(const char* in) {
  register const char* t=in;
  for (;;) {
    if (!*t) break;
                    ++t;
    if (!*t) break;
                    ++t;
    if (!*t) break;
                    ++t;
    if (!*t) break;
                    ++t;
  }
  return (size_t)(t-in);
}

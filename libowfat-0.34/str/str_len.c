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

#ifdef UNITTEST
#include <assert.h>
int main() {
  assert(str_len("foo")==3);
  assert(str_len("")==0);
  assert(str_len("fnord")==5);
  return 0;
}
#endif

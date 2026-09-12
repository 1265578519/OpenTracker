#include "str.h"

size_t str_rchr(const char *in, char needle) {
#if 1
  size_t i,j=-1;
  for (i=0; in[i]; ++i) if (in[i]==needle) j=i;
  return i<j ? i : j;
#else
  register const char* t=in;
  register const char c=needle;
  register const char* found=0;
  for (;;) {
    if (!*t) break;
                    if (*t==c) found=t;
                                        ++t;

    if (!*t) break;
                    if (*t==c) found=t;
                                        ++t;

    if (!*t) break;
                    if (*t==c) found=t;
                                        ++t;

    if (!*t) break;
                    if (*t==c) found=t;
                                        ++t;
  }
  return (size_t)((found?found:t)-in);
#endif
}

#ifdef UNITTEST
#include <assert.h>
int main() {
  assert(str_rchr("fnord",'r')==3);
  assert(str_rchr("frord",'r')==3);
  assert(str_rchr("fnord",'x')==5);
  return 0;
}
#endif

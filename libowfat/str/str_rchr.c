#include "str.h"

size_t str_rchr(const char *in, char needle) {
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
}

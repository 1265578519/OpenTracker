#include "str.h"

size_t str_chr(const char *in, char needle) {
  register const char* t=in;
  register const char c=needle;
  for (;;) {
    if (!*t || *t==c) break;
                             ++t;
    if (!*t || *t==c) break;
                             ++t;
    if (!*t || *t==c) break;
                             ++t;
    if (!*t || *t==c) break;
                             ++t;
  }
  return (size_t)(t-in);
}

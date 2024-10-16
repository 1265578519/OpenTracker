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

#ifdef UNITTEST
#include <assert.h>
#include <string.h>

int main() {
  char buf[100];
  strcpy(buf,"abcdefghijklmnopqrstuvwxyz");
  size_t i;
  for (i=0; i<26; ++i)
    assert(str_chr(buf,buf[i])==i);
  assert(str_chr(buf,'X')==26);
}
#endif

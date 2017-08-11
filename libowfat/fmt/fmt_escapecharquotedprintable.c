#include "fmt.h"

#ifdef UNITTEST
#undef UNITTEST
#include "fmt_tohex.c"
#define UNITTEST
#endif

size_t fmt_escapecharquotedprintable(char* dest,uint32_t ch) {
  if (ch>0xff) return 0;
  if (dest) {
    dest[0]='=';
    dest[2]=fmt_tohex(ch&0xf); ch>>=4;
    dest[1]=fmt_tohex(ch&0xf);
  }
  return 3;
}

#ifdef UNITTEST
#include <assert.h>
#include <string.h>

int main() {
  char buf[100];
  assert(fmt_escapecharquotedprintable(buf,'f')==3 && !memcmp(buf,"=66",3));
}
#endif

#include "fmt.h"
#include "haveinline.h"

static inline char tohex(char c) {
  return (char)(c>=10?c-10+'a':c+'0');
}

size_t fmt_xlong(char *dest,unsigned long i) {
  register unsigned long len,tmp;
  /* first count the number of bytes needed */
  for (len=1, tmp=i; tmp>15; ++len) tmp>>=4;
  if (dest)
    for (tmp=i, dest+=len; ; ) {
      *--dest = tohex(tmp&15);
      if (!(tmp>>=4)) break;
    }
  return len;
}

#ifdef UNITTEST
#include <assert.h>
#include <string.h>

int main() {
  char buf[100];
  assert(fmt_xlong(buf,0x12345)==5 && !memcmp(buf,"12345",5));
  return 0;
}
#endif

#include "fmt.h"

size_t fmt_8long(char *dest,unsigned long i) {
  register unsigned long len,tmp;
  /* first count the number of bytes needed */
  for (len=1, tmp=i; tmp>7; ++len) tmp>>=3;
  if (dest)
    for (tmp=i, dest+=len; ; ) {
      *--dest = (char)((tmp&7)+'0');
      if (!(tmp>>=3)) break;
    }
  return len;
}

#ifdef UNITTEST
#include <assert.h>
#include <string.h>

int main() {
  char buf[100];
  assert(fmt_8long(buf,012345)==5 && !memcmp(buf,"12345",5));
  assert(fmt_8long(buf,0)==1 && !memcmp(buf,"0",1));
  return 0;
}
#endif

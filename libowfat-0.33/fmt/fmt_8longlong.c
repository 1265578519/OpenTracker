#include "fmt.h"

size_t fmt_8longlong(char *dest,unsigned long long i) {
  register unsigned long len;
  unsigned long long tmp;
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
  assert(fmt_8longlong(buf,0)==1 && !memcmp(buf,"0",1));
  assert(fmt_8longlong(buf,0123456701234567)==15 && !memcmp(buf,"123456701234567",15));
  return 0;
}
#endif

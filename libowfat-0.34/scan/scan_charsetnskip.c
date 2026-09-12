#include "scan.h"
#include <ctype.h>

/* Return the number of leading chars in s that are in charset, but no
 * more than limit */
size_t scan_charsetnskip(const char *s,const char *charset,size_t limit) {
  register const char *t=s;
  register const char *u=t+limit;
  register const char* i;
  while (t<u) {
    for (i=charset; *i; ++i)
      if (*i==*t) break;
    if (*i!=*t) break;
    ++t;
  }
  return (size_t)(t-s);
}

#ifdef UNITTEST
#include <assert.h>

int main() {
  assert(scan_charsetnskip("fnord","fo",4)==1);
  assert(scan_charsetnskip("fnord","nf",4)==2);
  assert(scan_charsetnskip("gaord","nf",4)==0);
  assert(scan_charsetnskip("fnord","fnord",4)==4);
  return 0;
}
#endif

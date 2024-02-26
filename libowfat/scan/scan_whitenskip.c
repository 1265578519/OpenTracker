#include "scan.h"
#include <ctype.h>

size_t scan_whitenskip(const char *s,size_t limit) {
  register const char *t=s;
  register const char *u=t+limit;
  while (t<u && isspace(*t)) ++t;
  return (size_t)(t-s);
}

#ifdef UNITTEST
#include <assert.h>

int main() {
  assert(scan_whitenskip("foo",3)==0);
  assert(scan_whitenskip(" foo",4)==1);
  assert(scan_whitenskip("  foo",1)==1);
  assert(scan_whitenskip("  foo",2)==2);
  return 0;
}
#endif

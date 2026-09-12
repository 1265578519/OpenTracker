#include "scan.h"
#include <ctype.h>

size_t scan_noncharsetnskip(const char *s,const char *charset,size_t limit) {
  register const char *t=s;
  register const char *u=t+limit;
  register const char* i;
  while (t<u) {
    for (i=charset; *i; ++i)
      if (*i==*t) break;
    if (*i==*t) break;
    ++t;
  }
  return (size_t)(t-s);
}

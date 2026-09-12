#include "scan.h"
#include <ctype.h>

size_t scan_nonwhitenskip(const char *s,size_t limit) {
  register const char *t=s;
  register const char *u=t+limit;
  while (t<u && !isspace(*t)) ++t;
  return (size_t)(t-s);
}

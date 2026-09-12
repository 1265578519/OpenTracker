#include "str.h"

size_t str_copy(char *out,const char *in) {
  register char* s=out;
  register const char* t=in;
  for (;;) {
    if (!(*s=*t)) break;
                         ++s; ++t;
    if (!(*s=*t)) break;
                         ++s; ++t;
    if (!(*s=*t)) break;
                         ++s; ++t;
    if (!(*s=*t)) break;
                         ++s; ++t;
  }
  return (size_t)(s-out);
}

#ifdef UNITTEST
#include <assert.h>
#include <string.h>

int main() {
  char buf[100];
  memset(buf,1,sizeof(buf)); assert(str_copy(buf,"foo")==3 && !memcmp(buf,"foo",4));
}
#endif

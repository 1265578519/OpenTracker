#include "byte.h"

/* str_diff returns negative, 0, or positive, depending on whether the
 * string a[0], a[1], ..., a[n]=='\0' is lexicographically smaller than,
 * equal to, or greater than the string b[0], b[1], ..., b[m-1]=='\0'.
 * When the strings are different, str_diff does not read bytes past the
 * first difference. */
int str_diffn(const char* a, const char* b, size_t limit) {
  register const unsigned char* s=(const unsigned char*)a;
  register const unsigned char* t=(const unsigned char*)b;
  register const unsigned char* u=t+limit;
  register int j;
  j=0;
  for (;;) {
    if (t>=u || (j=(*s-*t)) || !*t) break;
                                           ++s; ++t;
    if (t>=u || (j=(*s-*t)) || !*t) break;
                                           ++s; ++t;
    if (t>=u || (j=(*s-*t)) || !*t) break;
                                           ++s; ++t;
    if (t>=u || (j=(*s-*t)) || !*t) break;
                                           ++s; ++t;
  }
  return j;
}

#ifdef UNITTEST
#include <assert.h>
#include <string.h>

int main() {
  assert(str_diffn("foo","foo",3)==0);
  assert(str_diffn("foo","fob",3)==('o'-'b'));
  assert(str_diffn("foo","foox",4)==(-'x'));
  assert(str_diffn("foox","foo",4)=='x');
  assert(str_diffn("foo","fob",2)==0);
}
#endif

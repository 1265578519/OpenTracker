#include "byte.h"

/* str_diff returns negative, 0, or positive, depending on whether the
 * string a[0], a[1], ..., a[n]=='\0' is lexicographically smaller than,
 * equal to, or greater than the string b[0], b[1], ..., b[m-1]=='\0'.
 * When the strings are different, str_diff does not read bytes past the
 * first difference. */
int str_diff(const char* a, const char* b) {
  register const unsigned char* s=(const unsigned char*)a;
  register const unsigned char* t=(const unsigned char*)b;
  register int j;
  for (;;) {
    if ((j=(*s-*t)) || !*t) break;
                                   ++s; ++t;
    if ((j=(*s-*t)) || !*t) break;
                                   ++s; ++t;
    if ((j=(*s-*t)) || !*t) break;
                                   ++s; ++t;
    if ((j=(*s-*t)) || !*t) break;
                                   ++s; ++t;
  }
  return j;
}

#ifdef UNITTEST
#include <assert.h>
#include <string.h>

int main() {
  assert(str_diff("foo","foo")==0);
  assert(str_diff("foo","fob")==('o'-'b'));
  assert(str_diff("foo","foox")==(-'x'));
  assert(str_diff("foox","foo")=='x');
}
#endif

#include "byte.h"

/* byte_chr returns the smallest integer i between 0 and len-1
 * inclusive such that one[i] equals needle, or len if not found. */
size_t byte_chr(const void* haystack, size_t len, char needle) {
  register char c=needle;
  register const char* s=haystack;
  register const char* t=s+len;
  for (;;) {
    if (s==t || *s==c) break;
                              ++s;
    if (s==t || *s==c) break;
                              ++s;
    if (s==t || *s==c) break;
                              ++s;
    if (s==t || *s==c) break;
                              ++s;
  }
  return (size_t)(s-(const char*)haystack);
}

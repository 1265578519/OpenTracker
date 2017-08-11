#include "byte.h"

/* byte_rchr returns the largest integer i between 0 and len-1 inclusive
 * such that one[i] equals needle, or len if not found. */
size_t byte_rchr(const void* haystack,size_t len,char needle) {
  register char c=needle;
  register const char* s=haystack;
  register const char* t=s+len;
  for (;;) {
    --t; if (s>t) break; if (*t==c) return (size_t)(t-s);
    --t; if (s>t) break; if (*t==c) return (size_t)(t-s);
    --t; if (s>t) break; if (*t==c) return (size_t)(t-s);
    --t; if (s>t) break; if (*t==c) return (size_t)(t-s);
  }
  return len;
}

#include "case.h"

int case_diffb(register const void* S,register size_t len,register const void* T)
{
  register unsigned char x;
  register unsigned char y;
  register const char* s=(const char*)S;
  register const char* t=(const char*)T;

  while (len > 0) {
    --len;
    x = *s++ - 'A';
    if (x <= 'Z' - 'A') x += 'a'; else x += 'A';
    y = *t++ - 'A';
    if (y <= 'Z' - 'A') y += 'a'; else y += 'A';
    if (x != y)
      return ((int)(unsigned int) x) - ((int)(unsigned int) y);
  }
  return 0;
}

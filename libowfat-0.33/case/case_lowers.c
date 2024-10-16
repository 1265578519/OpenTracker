#include "case.h"

void case_lowers(char *s) {
  unsigned char x;
  for (;;) {
    if (!(x=*s)) break;
    if ((x -= 'A') <= 'Z' - 'A') *s = x + 'a';
    ++s;
  }
}

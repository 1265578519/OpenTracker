#include "stralloc.h"

int stralloc_chop(stralloc* sa) {
  if (sa->len==0) return -1;
  --sa->len;
  return (unsigned char)(sa->s[sa->len]);
}

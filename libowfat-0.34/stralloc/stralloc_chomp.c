#include "stralloc.h"

int stralloc_chomp(stralloc* sa) {
  size_t max=sa->len;
  if (max>0) {
    register char x;
    --max;
    x=sa->s[max];
    if (x=='\n' || x=='\r') {
      if (x=='\n' && max>1 && sa->s[max-1]=='\r') {
	sa->len-=2;
	return 2;
      }
      --sa->len;
      return 1;
    }
  }
  return 0;
}

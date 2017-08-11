#include "stralloc.h"

void stralloc_init(stralloc* sa) {
  sa->s=0;
  sa->len=sa->a=0;
}

#include <stdlib.h>
#include "stralloc.h"

void stralloc_free(stralloc *sa) {
  if (sa->s) free(sa->s);
  sa->s=0;
  sa->a=sa->len=0;
}

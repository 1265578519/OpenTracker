#include <stdlib.h>
#include "array.h"

void array_reset(array* x) {
  if (x->p) free(x->p);
  x->p=0;
  x->allocated=x->initialized=0;
}

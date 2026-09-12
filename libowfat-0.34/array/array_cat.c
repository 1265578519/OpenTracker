#include "array.h"
#include "byte.h"

void array_cat(array* to,const array* const from) {
  if (from->allocated<0) {
    array_fail(to);
    return;
  }
  array_catb(to,from->p,from->initialized);
}

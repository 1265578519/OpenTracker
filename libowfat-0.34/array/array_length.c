#include "array.h"

int64 array_length(const array* const x,uint64 membersize) {
  if (x->allocated<0) return -1;
  return x->initialized/membersize;
}

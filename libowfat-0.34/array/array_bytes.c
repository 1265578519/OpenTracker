#include "array.h"

int64 array_bytes(const array* const x) {
  if (x->allocated<0) return -1;
  return x->initialized;
}

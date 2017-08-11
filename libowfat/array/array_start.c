#include "array.h"

void* array_start(const array* const x) {
  return x->p;
}

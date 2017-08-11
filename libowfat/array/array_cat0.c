#include "array.h"

void array_cat0(array* to) {
  static char zero;
  array_catb(to,&zero,1);
}

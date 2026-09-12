#include "scan.h"

size_t scan_xlong(const char *src,unsigned long *dest) {
  return scan_xlongn(src,((size_t)-1)/2,dest);
}

/* unit tested via scan_xlongn */

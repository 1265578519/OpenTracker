#include "scan.h"

size_t scan_8long(const char *src,unsigned long *dest) {
  return scan_8longn(src,(size_t)-1,dest);
}

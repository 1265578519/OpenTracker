#include "scan.h"

size_t scan_long(const char *src,long *dest) {
  return scan_longn(src,(size_t)(-1),dest);
}

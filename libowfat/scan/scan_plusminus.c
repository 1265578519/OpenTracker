#include "scan.h"

size_t scan_plusminus(const char *src,signed int *dest) {
  *dest=1;
  switch (*src) {
  case '-': *dest=-1;
  case '+': return 1; break;
  }
  return 0;
}

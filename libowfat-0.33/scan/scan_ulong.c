#include "scan.h"

size_t scan_ulong(const char* src,unsigned long int* dest) {
  return scan_ulongn(src,((size_t)-1)/2,dest);
}

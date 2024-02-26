#define NO_UINT64_MACROS
#include "uint64.h"
#include "uint32.h"

uint64 uint64_read(const char *in) {
  return uint32_read(in) | ((uint64)uint32_read(in+4)<<32);
}

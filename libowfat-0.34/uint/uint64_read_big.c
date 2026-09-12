#define NO_UINT64_MACROS
#include "uint64.h"
#include "uint32.h"

uint64 uint64_read_big(const char *in) {
  return ((uint64)uint32_read_big(in)<<32) | uint32_read_big(in+4);
}

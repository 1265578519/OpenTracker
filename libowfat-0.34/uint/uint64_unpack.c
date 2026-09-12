#define NO_UINT64_MACROS
#include "uint64.h"
#include "uint32.h"

void uint64_unpack(const char *in,uint64 *out) {
  *out = uint64_read(in);
}

#include "fmt.h"
#include "uint32.h"

size_t fmt_pb_type5_fixed32(char* dest,uint32_t l) {
  if (dest) uint32_pack(dest,l);
  return 4;
}

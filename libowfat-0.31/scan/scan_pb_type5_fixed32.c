#include "scan.h"
#include "uint32.h"

size_t scan_pb_type5_fixed32(const char* in,size_t len,uint32_t* d) {
  if (len<4) return 0;
  uint32_unpack(in,d);
  return 4;
}

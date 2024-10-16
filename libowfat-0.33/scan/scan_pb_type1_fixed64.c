#include "scan.h"
#include "uint64.h"

size_t scan_pb_type1_fixed64(const char* in,size_t len,uint64_t* d) {
  if (len<8) return 0;
  uint64_unpack(in,d);
  return 8;
}

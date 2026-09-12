#include "scan.h"

size_t scan_pb_type0_sint(const char* in,size_t len,signed long long* l) {
  unsigned long long m;
  size_t n=scan_varint(in,len,&m);
  if (!n) return 0;
  *l=(-(m&1)) ^ (m>>1);
  return n;
}

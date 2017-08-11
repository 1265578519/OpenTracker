#include "scan.h"

size_t scan_pb_tag(const char* in,size_t len, size_t* fieldno,unsigned char* type) {
  unsigned long long l;
  size_t n=scan_varint(in,len,&l);
  if (n==0) return 0;
  *type=l&7;
  *fieldno=(l>>3);
  return n;
}

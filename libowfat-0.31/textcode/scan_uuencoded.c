#include "textcode.h"

size_t scan_uuencoded(const char *src,char *dest,size_t *destlen) {
  size_t len;
  size_t tmp;
  register const unsigned char* s=(const unsigned char*) src;
  const char* orig=dest;
  if ((len=*s-' ')>64) return 0;
  len&=63;
  ++s;
  while (len>0) {
    if (s[0]-' '>64 || s[1]-' '>64 || s[2]-' '>64 || s[3]-' '>64) return 0;
    tmp=(((s[0]-' ')&077) << (3*6)) +
        (((s[1]-' ')&077) << (2*6)) +
        (((s[2]-' ')&077) << (1*6)) +
        (((s[3]-' ')&077));
    s+=4;
    if (len) { *dest=tmp>>16; ++dest; --len; }
    if (len) { *dest=tmp>>8; ++dest; --len; }
    if (len) { *dest=tmp&0xff; ++dest; --len; }
  }
  *destlen=dest-orig;
  return (const char*)s-src;
}

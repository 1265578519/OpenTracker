#include "textcode.h"

size_t scan_uuencoded(const char *src,char *dest,size_t *destlen) {
  size_t len;
  size_t tmp;
  register const unsigned char* s=(const unsigned char*) src;
  size_t i=0;
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
    if (len) { if (dest) dest[i++]=tmp>>16; --len; }
    if (len) { if (dest) dest[i++]=tmp>>8; --len; }
    if (len) { if (dest) dest[i++]=tmp&0xff; --len; }
  }
  if (destlen) *destlen=i;
  return (const char*)s-src;
}

#ifdef UNITTEST

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main() {
  char buf[100];
  size_t i;
  memset(buf,0,100);
  assert(scan_uuencoded("&9FYO<F0*",buf,&i)==9 && i==6 && !memcmp(buf,"fnord\n",7));
  memset(buf,0,100);
  assert(scan_uuencoded("%9FYO<F0`",buf,&i)==9 && i==5 && !memcmp(buf,"fnord",6));
  return 0;
}

#endif

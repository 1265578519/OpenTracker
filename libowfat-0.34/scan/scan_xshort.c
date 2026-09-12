#include "scan.h"

#ifdef UNITTEST
#undef UNITTEST
#include "scan_fromhex.c"
#define UNITTEST
#endif

size_t scan_xshort(const char* src,unsigned short* dest) {
  register const char *tmp=src;
  register unsigned short l=0;
  register unsigned char c;
  while ((l>>(sizeof(l)*8-4))==0 && (c=(unsigned char)scan_fromhex((unsigned char)*tmp))<16) {
    l=(unsigned short)((l<<4)+c);
    ++tmp;
  }
  *dest=l;
  return (size_t)(tmp-src);
}

#ifdef UNITTEST
#include <assert.h>

int main() {
  unsigned short i;
  assert(scan_xshort("fefe",&i)==4 && i==0xfefe);
  assert(scan_xshort("0xfefe",&i)==1);	// 0x not supported, will scan the 0
  assert(scan_xshort("+fefe",&i)==0);
  assert(scan_xshort("fefec0de",&i)==4 && i==0xfefe);	// test truncation
  return 0;
}
#endif

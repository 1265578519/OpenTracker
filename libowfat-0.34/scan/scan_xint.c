#include "scan.h"

#ifdef UNITTEST
#undef UNITTEST
#include "scan_fromhex.c"
#define UNITTEST
#endif

size_t scan_xint(const char* src,unsigned int* dest) {
  register const char *tmp=src;
  register unsigned int l=0;
  register unsigned char c;
  while ((l>>(sizeof(l)*8-4))==0 && (c=(unsigned char)scan_fromhex((unsigned char)*tmp))<16) {
    l=(l<<4)+c;
    ++tmp;
  }
  *dest=l;
  return (size_t)(tmp-src);
}

#ifdef UNITTEST
#include <assert.h>

int main() {
  unsigned int i;
  assert(scan_xint("fefe",&i)==4 && i==0xfefe);
  assert(scan_xint("0xfefe",&i)==1);	// 0x not supported, will scan the 0
  assert(scan_xint("+fefe",&i)==0);
  assert(scan_xint("fefec0de",&i)==8 && i==0xfefec0de);
  assert(scan_xint("fefec0debaad",&i)==8 && i==0xfefec0de);	// test truncation
  return 0;
}
#endif

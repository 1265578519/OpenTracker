#include "scan.h"

#ifdef UNITTEST
#undef UNITTEST
#include "scan_fromhex.c"
#define UNITTEST
#endif

size_t scan_xlongn(const char *src,size_t n,unsigned long *dest) {
  register const char *tmp=src;
  register unsigned long l=0;
  register unsigned char c;
  while (n-->0 && (l>>(sizeof(l)*8-4))==0 && (c=(unsigned char)scan_fromhex((unsigned char)*tmp))<16) {
    l=(l<<4)+c;
    ++tmp;
  }
  *dest=l;
  return (size_t)(tmp-src);
}

#ifdef UNITTEST
#include <assert.h>

int main() {
  unsigned long i;
  assert(scan_xlongn("fefe",4,&i)==4 && i==0xfefe);
  assert(scan_xlongn("0xfefe",6,&i)==1);	// 0x not supported, will scan the 0
  assert(scan_xlongn("+fefe",5,&i)==0);
  assert(scan_xlongn("fefec0de",7,&i)==7 && i==0xfefec0d);
  assert(scan_xlongn("fefec0de",8,&i)==8 && i==0xfefec0de);
  assert(scan_xlongn("fefec0de",9,&i)==8 && i==0xfefec0de);
  if (sizeof(i)==4)
    assert(scan_xlongn("fefec0debaad",14,&i)==8 && i==0xfefec0de);	// test truncation
  else if (sizeof(i)==8)
    assert(scan_xlongn("fefec0debaadc0debl06",40,&i)==16 && i==0xfefec0debaadc0de);	// test truncation
  return 0;
}
#endif

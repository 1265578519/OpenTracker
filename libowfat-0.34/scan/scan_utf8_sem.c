#include "scan.h"

size_t scan_utf8_sem(const char* in,size_t len,uint32_t* num) {
  uint32_t tmp=0;
  size_t r=scan_utf8(in,len,&tmp);
  if (r>0) {
    if (tmp>=0xd800 && tmp<=0xdfff) return 0;
    if ((tmp&0xfffe)==0xfffe) return 0;
    if (tmp>=0xfdd0 && tmp<=0xfdef) return 0;
    *num=tmp;
  }
  return r;
}

#ifdef UNITTEST
#include <assert.h>

#undef UNITTEST
#include "scan/scan_utf8.c"

int main() {
  uint32_t l;
  /* rest of scan_utf8 tested in scan_utf8.c unit tests */
  assert(scan_utf8_sem("\xed\xa0\x80",4,&l)==0);
  return 0;
}
#endif

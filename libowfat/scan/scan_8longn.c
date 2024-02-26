#include "scan.h"

size_t scan_8longn(const char *src,size_t n,unsigned long *dest) {
  /* make a copy of src so we can return the number of bytes we progressed */
  register const char *tmp=src;
  register unsigned long l=0;
  register unsigned char c;
  /* *tmp - '0' can be negative, but casting to unsigned char makes
   * those cases positive and large; that means we only need one
   * comparison.  This trick is no longer needed on modern compilers,
   * but we also want to produce good code on old compilers :) */
  while (n-->0 && (c=(unsigned char)(*tmp-'0'))<8) {
    /* overflow check; for each digit we multiply by 8 and then add the
     * digit; 0-7 needs 3 bits of storage, so we need to check if the
     * uppermost 3 bits of l are empty. Do it by shifting to the right */
    if (l>>(sizeof(l)*8-3)) break;
    l=l*8+c;
    ++tmp;
  }
  *dest=l;
  return (size_t)(tmp-src);
}

#ifdef UNITTEST
#include <assert.h>

int main() {
  unsigned long i;
  assert(scan_8longn("1234\0""67890",10,&i)==4 && i==01234);
  assert(scan_8longn("12345678",9,&i)==7 && i==01234567);
  if (sizeof(long)==sizeof(int)) {
    assert(scan_8longn("37777777777",12,&i)==11 && i==0xffffffff);
    assert(scan_8longn("40000000000",12,&i)==10 && i==04000000000);
  } else if (sizeof(long)==sizeof(long long)) {
    assert(scan_8longn("1777777777777777777777",23,&i)==22 && i==0xffffffffffffffffull);
    assert(scan_8longn("2000000000000000000000",23,&i)==21 && i==0200000000000000000000ull);
  }
  assert(scan_8longn("-4",3,&i)==0 && i==0);
  assert(scan_8longn("01234",6,&i)==5 && i==01234);
  assert(scan_8longn("1234",2,&i)==2 && i==012);
  return 0;
}
#endif

#include "scan.h"

#ifdef UNITTEST
#undef UNITTEST
#include "scan_8longn.c"
#define UNITTEST
#endif

size_t scan_8long(const char *src,unsigned long *dest) {
  return scan_8longn(src,(size_t)-1,dest);
}

#ifdef UNITTEST
#include <assert.h>

int main() {
  unsigned long i;
  assert(scan_8long("1234",&i)==4 && i==01234);
  assert(scan_8long("12345678",&i)==7 && i==01234567);
  if (sizeof(long)==sizeof(int)) {
    assert(scan_8long("37777777777",&i)==11 && i==0xffffffff);
    assert(scan_8long("40000000000",&i)==10 && i==04000000000);
  } else if (sizeof(long)==sizeof(long long)) {
    assert(scan_8long("1777777777777777777777",&i)==22 && i==0xffffffffffffffffull);
    assert(scan_8long("2000000000000000000000",&i)==21 && i==0200000000000000000000ull);
  }
  assert(scan_8long("-4",&i)==0 && i==0);
  assert(scan_8long("01234",&i)==5 && i==01234);
  return 0;
}
#endif

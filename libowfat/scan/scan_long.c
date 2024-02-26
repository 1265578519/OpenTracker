#include "scan.h"

#ifdef UNITTEST
#undef UNITTEST
#include "scan_longn.c"
#define UNITTEST
#endif

size_t scan_long(const char *src,long *dest) {
  return scan_longn(src,(size_t)(-1),dest);
}

#ifdef UNITTEST
#include <assert.h>

int main() {
  long i;
  assert(scan_long("1234",&i)==4 && i==1234);
  assert(scan_long("-1234",&i)==5 && i==-1234);
  assert(scan_long("+1234",&i)==5 && i==1234);
  assert(scan_long("2147483647",&i)==10 && i==2147483647);	// INT_MAX
  assert(scan_long("-2147483648",&i)==11 && i==-2147483648);	// INT_MIN
  if (sizeof(int) == sizeof(long)) {
    assert(scan_long("4294967295",&i)==9 && i==429496729);	// overflow
    assert(scan_long("2147483648",&i)==9 && i==214748364);	// overflow
    assert(scan_long("-2147483649",&i)==10 && i==-214748364);	// underflow
  } else {
    assert(scan_long("9223372036854775807",&i)==19 && i==0x7fffffffffffffffull);	// LONG_MAX
    assert(scan_long("-9223372036854775808",&i)==20 && i==(long long)0x8000000000000000ull);	// LONG_MIN
  }
  return 0;
}
#endif

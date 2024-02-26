#include "scan.h"

static const unsigned long maxlong = ((unsigned long)-1)>>1;

#ifdef UNITTEST
#undef UNITTEST
#include "scan_ulonglong.c"
#define UNITTEST
#endif

size_t scan_longlong(const char* src,signed long long* dest) {
  size_t i,o;
  unsigned long long l;
  char c=src[0];
  unsigned int neg=c=='-';
  o=c=='-' || c=='+';
  if ((i=scan_ulonglong(src+o,&l))) {
    if (i>0 && l>maxlong+neg) {
      l/=10;
      --i;
    }
    if (i+o) *dest=(signed long long)(c=='-'?-l:l);
    return i+o;
  } return 0;
}

#ifdef UNITTEST
#include <assert.h>

int main() {
  long long i;
  assert(scan_longlong("1234",&i)==4 && i==1234);
  assert(scan_longlong("-1234",&i)==5 && i==-1234);
  assert(scan_longlong("+1234",&i)==5 && i==1234);
  assert(scan_longlong("2147483647",&i)==10 && i==2147483647);	// INT_MAX
  assert(scan_longlong("-2147483648",&i)==11 && i==-2147483648);	// INT_MIN
  assert(scan_longlong("9223372036854775807",&i)==19 && i==0x7fffffffffffffffull);	// LONG_MAX
  assert(scan_longlong("-9223372036854775808",&i)==20 && i==(long long)0x8000000000000000ull);	// LONG_MIN
  return 0;
}
#endif

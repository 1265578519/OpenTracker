#include "scan.h"

/* this is cut and paste from scan_ulong instead of calling scan_ulong
 * because this way scan_ushort can abort scanning when the number would
 * not fit into an unsigned short (as opposed to not fitting in an
 * unsigned long) */

size_t scan_ushort(const char* src,unsigned short* dest) {
  if (sizeof(unsigned short) == sizeof(unsigned int)) {
    /* a good optimizing compiler should remove the else clause when not
     * needed */
    return scan_uint(src,(unsigned int*)dest);
  } if (sizeof(unsigned short) < sizeof(unsigned long)) {
    /* this is the regular case */
    const char* cur;
    unsigned short l;
    for (cur=src,l=0; *cur>='0' && *cur<='9'; ++cur) {
      unsigned long tmp=l*10ul+*cur-'0';
      if ((unsigned short)tmp != tmp) break;
      l=tmp;
    }
    if (cur>src) *dest=l;
    return (size_t)(cur-src);
  } else {
    /* the C standard says that sizeof(short) <= sizeof(unsigned int) <=
     * sizeof(unsigned long); this can never happen. Provoke a compile
     * error if it does */
    char compileerror[sizeof(unsigned long)-sizeof(unsigned short)];
    (void)compileerror;
  }
}

#ifdef UNITTEST
#include <assert.h>

int main() {
  unsigned short i;
  assert(scan_ushort("1234",&i)==4 && i==1234);
  assert(scan_ushort("-1",&i)==0);
  if (sizeof(short)==2) {
    assert(scan_ushort("123456",&i)==5 && i==12345);
  }
  return 0;
}
#endif

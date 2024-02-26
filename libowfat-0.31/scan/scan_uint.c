#include "scan.h"

/* this is cut and paste from scan_ulong instead of calling scan_ulong
 * because this way scan_uint can abort scanning when the number would
 * not fit into an unsigned int (as opposed to not fitting in an
 * unsigned long) */

size_t scan_uint(const char* src,unsigned int* dest) {
  if (sizeof(unsigned int) == sizeof(unsigned long)) {
    /* a good optimizing compiler should remove the else clause when not
     * needed */
    return scan_ulong(src,(unsigned long*)dest);
  } else if (sizeof(unsigned int) < sizeof(unsigned long)) {
    const char* cur;
    unsigned int l;
    for (cur=src,l=0; *cur>='0' && *cur<='9'; ++cur) {
      unsigned long tmp=l*10ul+*cur-'0';
      if ((unsigned int)tmp != tmp) break;
      l=tmp;
    }
    if (cur>src) *dest=l;
    return (size_t)(cur-src);
  } else {
    /* the C standard says that sizeof(short) <= sizeof(unsigned int) <=
     * sizeof(unsigned long); this can never happen. Provoke a compile
     * error if it does */
    char compileerror[sizeof(unsigned long)-sizeof(unsigned int)];
    (void)compileerror;
  }
}

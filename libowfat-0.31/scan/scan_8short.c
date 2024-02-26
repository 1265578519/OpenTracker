#include "scan.h"

size_t scan_8short(const char* src,unsigned short* dest) {
  /* make a copy of src so we can return the number of bytes we progressed */
  register const char *tmp=src;
  /* this is called l because we copy and pasted from 8long, where l
   * stood for "long" */
  register unsigned short l=0;
  register unsigned char c;
  /* *tmp - '0' can be negative, but casting to unsigned char makes
   * those cases positive and large; that means we only need one
   * comparison.  This trick is no longer needed on modern compilers,
   * but we also want to produce good code on old compilers :) */
  while ((c=(unsigned char)(*tmp-'0'))<8) {
    /* overflow check; for each digit we multiply by 8 and then add the
     * digit; 0-7 needs 3 bits of storage, so we need to check if the
     * uppermost 3 bits of l are empty. Do it by shifting to the right */
    if (l>>(sizeof(l)*8-3)) break;
    l=(unsigned short)(l*8+c);
    ++tmp;
  }
  *dest=l;
  return (size_t)(tmp-src);
}

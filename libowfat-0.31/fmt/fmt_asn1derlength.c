#include "fmt.h"

/* write int in least amount of bytes, return number of bytes */
/* as used in ASN.1 length */
size_t fmt_asn1derlength(char* dest,unsigned long long l) {
  /* encoding is either l%128 or (0x80+number of bytes,bytes) */
  size_t needed=(sizeof l),i;
  if (l<128) {
    if (dest) *dest=l&0x7f;
    return 1;
  }
  for (i=1; i<needed; ++i)
    if (!(l>>(i*8)))
      break;
  if (dest) {
    size_t j=i;
    *dest=(char)(0x80+i); ++dest;
    while (j) {
      --j;
      *dest=(char)((l>>(j*8))&0xff);
      ++dest;
    }
  }
  return i+1;
}

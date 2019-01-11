#include "fmt.h"

/* write int in least amount of bytes, return number of bytes */
/* as used in ASN.1 DER tag */
size_t fmt_asn1dertag(char* dest,unsigned long long l) {
  /* high bit says if more bytes are coming, lower 7 bits are payload; big endian */
  size_t n=0,i;
  unsigned long long t;
  for (t=l, n=1; t>0x7f; t>>=7) ++n;
  for (i=0; i<n; ++i) {
    if (dest) dest[n-i-1]=(char)(((i!=0)<<7) | (char)(l&0x7f));
    l>>=7;
  }
  return i;
}

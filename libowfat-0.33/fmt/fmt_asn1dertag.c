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

#ifdef UNITTEST
#include <assert.h>
#include <string.h>

char buf[100];
void zap() { size_t i; for (i=0; i<sizeof(buf); ++i) buf[i]='_'; }

int main() {
  assert(fmt_asn1dertag(NULL,0)==1);
  zap(); assert(fmt_asn1dertag(buf,0)==1 && !memcmp(buf,"\x00_",2));
  assert(fmt_asn1dertag(NULL,0xc2)==2);
  zap(); assert(fmt_asn1dertag(buf,0xc2)==2 && !memcmp(buf,"\x81\x42_",3));
  return 0;
}
#endif

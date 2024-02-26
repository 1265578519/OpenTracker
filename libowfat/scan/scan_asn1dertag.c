#include "scan.h"

/* ASN.1 DER tag encoding parser.
 * Write value as big endian series of bytes, each containing seven
 * bits.  In each byte except the last, set the highest bit.
 * Examples:
 *   0 -> 0x00
 *   5 -> 0x05
 *   0xc2 -> 0x81 0x42 */

size_t scan_asn1dertag(const char* src,size_t len,unsigned long long* length) {
  size_t n;
  unsigned long long l=0;
  if (len==0 || (unsigned char)src[0]==0x80) return 0;	// catch non-minimal encoding
  for (n=0; n<len; ++n) {
    /* make sure we can represent the stored number in l */
    if (l>>(sizeof(l)*8-7)) return 0;	// catch integer overflow
    l=(l<<7) | (src[n]&0x7f);
    /* if high bit not set, this is the last byte in the sequence */
    if (!(src[n]&0x80)) {
      *length=l;
      return n+1;
    }
  }
  return 0;
}

#ifdef UNITTEST
#include <assert.h>

int main() {
  unsigned long long i;
  assert(scan_asn1dertag("\x00",1,&i)==1 && i==0);
  assert(scan_asn1dertag("\x05",1,&i)==1 && i==5);
  assert(scan_asn1dertag("\x81\x42",2,&i)==2 && i==0xc2);
  assert(scan_asn1dertag("\x05",0,&i)==0);	// truncated
  assert(scan_asn1dertag("\x81\x42",1,&i)==0);	// truncated
  assert(scan_asn1dertag("\x80\x05",2,&i)==0);	// non-minimal encoding
  assert(scan_asn1dertag("\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff",10,&i)==0);	// value does not fit
  return 0;
}
#endif

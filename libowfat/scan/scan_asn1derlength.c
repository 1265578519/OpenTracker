#include "scan.h"

/* ASN.1 DER encoded length:
 * if (value<=0x80):
 *   emit value;
 * otherwise:
 *   emit 0x80+bytes_that_follow
 *   emit all the bytes in the number, number saved big endian
 * examples:
 *   5 -> 0x05
 *   0xc2 -> 0x81 0xc2
 *   0x123 -> 0x82 0x01 0x23
 *   0x12345 -> 0x83 0x01 0x23 0x45
 */

/* We provide two functions.
 * One that only parses the length value (scan_asn1derlengthvalue), and
 * one that also makes sure that as many bytes as specified by the
 * length are in the input buffer (scan_asn1derlength).
 * If you are trying to parse ASN.1, use scan_asn1derlength.
 * If you just want to use the integer encoding format used by ASN.1 DER
 * for lengths, use scan_asn1derlengthvalue. */

size_t scan_asn1derlengthvalue(const char* src,size_t len,unsigned long long* value) {
  if (len==0 || len>=-(uintptr_t)src) return 0;
  unsigned int i,c=*src;
  unsigned long long l;
  if ((c&0x80)==0) {
    *value=c;
    return 1;
  }
  /* Highest bit set: lower 7 bits is the length of the length value in bytes. */
  c&=0x7f;
  if (!c) return 0;	/* length 0x80 means indefinite length encoding, not supported here */
  l=(unsigned char)src[1];
  if (l==0) return 0;	/* not minimally encoded: 0x82 0x00 0xc2 instead of 0x81 0xc2 */
  if (c>sizeof(l)) return 0;	/* too many bytes, does not fit into target integer type */
  if (c+1>len) return 0;	/* not enough data in input buffer */
  for (i=2; i<=c; ++i)
    l=l*256+(unsigned char)src[i];
  if (l<0x7f) return 0;	/* not minimally encoded: 0x81 0x70 instead of 0x70 */
  *value=l;
  return i;
}

size_t scan_asn1derlength(const char* src,size_t len,unsigned long long* value) {
  unsigned long long l;
  size_t i=scan_asn1derlengthvalue(src,len,&l);
  if (!i || l > len-i) return 0;		/* make sure data would fit into buffer */
  *value=l;
  return i;
}

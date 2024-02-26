#include "fmt.h"

size_t scan_utf8(const char* in,size_t len,uint32_t* num) {
  uint32_t i,k,m;
  const char* orig=in;
  if (len==0) return 0;
  i=(*(unsigned char*)in++);	/* grab first byte */
  if (i>=0xfe ||		/* 0xfe and 0xff are invalid encodings in utf-8 for the first byte */
      (i&0xc0)==0x80) return 0;	/* first bits being 10 marks continuation chars, invalid sequence for first byte */
  for (k=0; i&0x80; i<<=1, ++k);	/* count leading 1 bits */
  if (!k) {
    if (num) *num=i;
    return 1;
  }
  if (k>len) return 0;
  i=(i&0xff)>>k;		/* mask the leading 1 bits */
  /* The next part is a little tricky.
   * UTF-8 says that the encoder has to choose the most efficient
   * encoding, and the decoder has to reject other encodings.  The
   * background is that attackers encoded '/' not as 0x2f but as 0xc0
   * 0xaf, and that evaded bad security checks just scan for the '/'
   * byte in pathnames.
   * At this point k contains the number of bytes, so k-1 is the number
   * of continuation bytes.  For each additional continuation byte, we
   * gain 6 bits of storage space, but we lose one in the signalling in
   * the initial byte.  So we have 6 + (k-1) * 5 bits total storage
   * space for this encoding.  The minimum value for k bytes is the
   * maximum number for k-1 bytes plus 1.  If the previous encoding has
   * 11 bits, its maximum value is 11 1-bits or 0x7ff, and the minimum
   * value we are looking for is 0x800 or 1<<11.  For 2 bytes, UTF-8 can
   * encode 11 bits, after that each additional byte gains 5 more bits.
   * So for k>2, we want
   *   1 << (11+(k-3)*5)
   * or optimized to get rid of the -3
   *   1 << (k*5-4)
   * but for k==2 the delta is 4 bits (not 5), so we want
   *   1 << 7
   * abusing the fact that a boolean expression evaluates to 0 or 1, the
   * expression can be written as
   *   1 << (k*5-4+(k==2))
   */
  m=((uint32_t)1<<(k*5-4+(k==2)));
  while (k>1) {
    if ((*in&0xc0)!=0x80) return 0;
    i=(i<<6) | ((*in++)&0x3f);
    --k;
  }
  if (i<m) return 0;	/* if the encoded value was less than m, reject */
  if (num) *num=i;
  return (size_t)(in-orig);
}


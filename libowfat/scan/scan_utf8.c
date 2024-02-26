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

#ifdef UNITTEST
#include <assert.h>
#include "fmt/fmt_utf8.c"

int main() {
  char buf[100];
  uint32_t l;
  unsigned int i;
  /* first positive testing for the various lengths */
  l=fmt_utf8(buf,0); assert(l == 1 && scan_utf8(buf,l+1,&l)==1 && l==0);
  l=fmt_utf8(buf,0x80); assert(l == 2 && scan_utf8(buf,l+1,&l)==2 && l==0x80);
  l=fmt_utf8(buf,0x800); assert(l == 3 && scan_utf8(buf,l+1,&l)==3 && l==0x800);
  l=fmt_utf8(buf,0x10000); assert(l == 4 && scan_utf8(buf,l+1,&l)==4 && l==0x10000);
  l=fmt_utf8(buf,0x200000); assert(l == 5 && scan_utf8(buf,l+1,&l)==5 && l==0x200000);
  l=fmt_utf8(buf,0x4000000); assert(l == 6 && scan_utf8(buf,l+1,&l)==6 && l==0x4000000);
  /* corner cases */
  l=fmt_utf8(buf,0x7f); assert(l == 1 && scan_utf8(buf,l+1,&l)==1 && l==0x7f);
  l=fmt_utf8(buf,0x7ff); assert(l == 2 && scan_utf8(buf,l+1,&l)==2 && l==0x7ff);
  l=fmt_utf8(buf,0xffff); assert(l == 3 && scan_utf8(buf,l+1,&l)==3 && l==0xffff);
  l=fmt_utf8(buf,0x1fffff); assert(l == 4 && scan_utf8(buf,l+1,&l)==4 && l==0x1fffff);
  l=fmt_utf8(buf,0x3ffffff); assert(l == 5 && scan_utf8(buf,l+1,&l)==5 && l==0x3ffffff);
  l=fmt_utf8(buf,0x7fffffff); assert(l == 6 && scan_utf8(buf,l+1,&l)==6 && l==0x7fffffff);
  /* more corner cases */
  l=fmt_utf8(buf,0xd7ff); assert(l == 3 && scan_utf8(buf,l+1,&l)==3 && l==0xd7ff);
  l=fmt_utf8(buf,0xe000); assert(l == 3 && scan_utf8(buf,l+1,&l)==3 && l==0xe000);
  l=fmt_utf8(buf,0xfffd); assert(l == 3 && scan_utf8(buf,l+1,&l)==3 && l==0xfffd);
  l=fmt_utf8(buf,0x10ffff); assert(l == 4 && scan_utf8(buf,l+1,&l)==4 && l==0x10ffff);
  l=fmt_utf8(buf,0x110000); assert(l == 4 && scan_utf8(buf,l+1,&l)==4 && l==0x110000);

  /* now negative testing */
  /* start off with some continuation bytes outside a sequence */
  for (i=0x80; i<=0xbf; ++i) {
    buf[0]=i;
    assert(scan_utf8(buf,2,&l)==0);
  }

  /* now check lonely sequence start characters */
  buf[1]=' ';
  for (i=0xc0; i<=0xfd; ++i) {
    buf[0]=i;
    assert(scan_utf8(buf,2,&l)==0);
  }

  /* FE and FF are reserved for UTF-16 endianness detection*/
  assert(scan_utf8("\xfe\xff",3,&l)==0);
  assert(scan_utf8("\xff\xfe",3,&l)==0);

  /* now check some truncated sequences */
  l=fmt_utf8(buf,0); assert(l == 1 && scan_utf8(buf,l-1,&l)==0);
  l=fmt_utf8(buf,0x80); assert(l == 2 && scan_utf8(buf,l-1,&l)==0);
  l=fmt_utf8(buf,0x800); assert(l == 3 && scan_utf8(buf,l-1,&l)==0);
  l=fmt_utf8(buf,0x10000); assert(l == 4 && scan_utf8(buf,l-1,&l)==0);
  l=fmt_utf8(buf,0x200000); assert(l == 5 && scan_utf8(buf,l-1,&l)==0);
  l=fmt_utf8(buf,0x4000000); assert(l == 6 && scan_utf8(buf,l-1,&l)==0);

  /* now truncate in another way */
  l=fmt_utf8(buf,0x80); buf[l-1]=' '; assert(l == 2 && scan_utf8(buf,l+1,&l)==0);
  l=fmt_utf8(buf,0x800); buf[l-1]=' '; assert(l == 3 && scan_utf8(buf,l+1,&l)==0);
  l=fmt_utf8(buf,0x10000); buf[l-1]=' '; assert(l == 4 && scan_utf8(buf,l+1,&l)==0);
  l=fmt_utf8(buf,0x200000); buf[l-1]=' '; assert(l == 5 && scan_utf8(buf,l+1,&l)==0);
  l=fmt_utf8(buf,0x4000000); buf[l-1]=' '; assert(l == 6 && scan_utf8(buf,l+1,&l)==0);

  /* now some not minimally encoded utf-8 sequences */
  assert(scan_utf8("\xc0\x80",3,&l)==0);
  assert(scan_utf8("\xe0\x80\x80",4,&l)==0);
  assert(scan_utf8("\xf0\x80\x80\x80",5,&l)==0);
  assert(scan_utf8("\xf8\x80\x80\x80\x80",6,&l)==0);
  assert(scan_utf8("\xfc\x80\x80\x80\x80\x80",7,&l)==0);
  return 0;
}
#endif

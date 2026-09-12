#include <assert.h>
#include "scan.h"
#include "fmt.h"

/* http://www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-test.txt */

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

  /* now check that scan_utf8_sem rejects surrogate pairs */
  assert(scan_utf8_sem("\xed\xa0\x80",4,&l)==0);

  /* now some not minimally encoded utf-8 sequences */
  assert(scan_utf8("\xc0\x80",3,&l)==0);
  assert(scan_utf8("\xe0\x80\x80",4,&l)==0);
  assert(scan_utf8("\xf0\x80\x80\x80",5,&l)==0);
  assert(scan_utf8("\xf8\x80\x80\x80\x80",6,&l)==0);
  assert(scan_utf8("\xfc\x80\x80\x80\x80\x80",7,&l)==0);
}

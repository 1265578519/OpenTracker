#include "fmt.h"

/* This is NOT fmt_escapexml, which will escape everything, whether it
 * needs escaping or not. This will only escape what needs escaping, and
 * reject invalid inputs */
size_t fmt_xmlescape(char* dest,uint32_t ch) {
  char* x;
  char buf[6];
  size_t n;
/*
From http://en.wikipedia.org/wiki/XML#Valid_characters

Unicode code points in the following ranges are valid in XML 1.0 documents:
  U+0009, U+000A, U+000D: these are the only C0 controls accepted in XML 1.0;
  U+0020–U+D7FF, U+E000–U+FFFD: this excludes some (not all) non-characters in the BMP (all surrogates, U+FFFE and U+FFFF are forbidden);
  U+10000–U+10FFFF: this includes all code points in supplementary planes, including non-characters.
*/
  if (ch==0 || (ch>=0xd780 && ch<=0xdfff) || ch==0xfffe || ch==0xffff || ch>0x10ffff) return 0;
  if (ch<0x20 && ch!=9 && ch!=0xa && ch!=0xd) {
    buf[0]='&';
    buf[1]='#';
    buf[2]='x';
    n=3+fmt_xlong(buf+3,ch);
    buf[n++]=';';
    x=buf;
  } else
    switch (ch) {
    case '&':
      x="&amp;"; n=5;
      break;
    case '<':
      x="&lt;"; n=4;
      break;
    default:
      return fmt_utf8(dest,ch);
    }
  if (dest) {
    size_t i;
    for (i=0; i<n; ++i)
      dest[i]=x[i];
  }
  return n;
}

#ifdef UNITTEST
#undef UNITTEST

#include "fmt_utf8.c"
#include "fmt_xlong.c"

#include <assert.h>
#include <string.h>

int main() {
  char buf[100];
  buf[0]=0x78;
  assert(fmt_xmlescape(buf,0) == 0 && buf[0]==0x78);	// 0 not allowed
  assert(fmt_xmlescape(buf,0xd800) == 0 && buf[0]==0x78);	// surrogate pairs not allowed
  assert(fmt_xmlescape(buf,0xdfff) == 0 && buf[0]==0x78);	// surrogate pairs not allowed
  assert(fmt_xmlescape(buf,0xfffe) == 0 && buf[0]==0x78);	// 0xfffe and 0xffff forbidden
  assert(fmt_xmlescape(buf,0xffff) == 0 && buf[0]==0x78);
  assert(fmt_xmlescape(buf,0x110000) == 0 && buf[0]==0x78);	// too large
  buf[1]=0x79;
  assert(fmt_xmlescape(buf,9) == 1 && buf[0]==9 && buf[1]==0x79);	// \t OK
  assert(fmt_xmlescape(buf,10) == 1 && buf[0]==10 && buf[1]==0x79);	// \n OK
  assert(fmt_xmlescape(buf,13) == 1 && buf[0]==13 && buf[1]==0x79);	// \r OK
  buf[5]=0x77;
  assert(fmt_xmlescape(buf,14) == 5 && !memcmp(buf,"&#xe;\x77",6));	// other control chars not OK
  assert(fmt_xmlescape(buf,'&') == 5 && !memcmp(buf,"&amp;\x77",6));	// & -> &amp;
  assert(fmt_xmlescape(buf,'<') == 4 && !memcmp(buf,"&lt;;\x77",6));	// < -> &lt;
}
#endif

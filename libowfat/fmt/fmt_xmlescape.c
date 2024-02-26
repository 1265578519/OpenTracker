#include "fmt.h"

size_t fmt_xmlescape(char* dest,uint32_t ch) {
  char* x;
  size_t n;
/*
From http://en.wikipedia.org/wiki/XML#Valid_characters

Unicode code points in the following ranges are valid in XML 1.0 documents:
  U+0009, U+000A, U+000D: these are the only C0 controls accepted in XML 1.0;
  U+0020–U+D7FF, U+E000–U+FFFD: this excludes some (not all) non-characters in the BMP (all surrogates, U+FFFE and U+FFFF are forbidden);
  U+10000–U+10FFFF: this includes all code points in supplementary planes, including non-characters.
*/
  if (ch==0 || (ch>=0xd780 && ch<=0xdfff) || ch==0xfffe || ch==0xffff || ch>0x10ffff) return 0;
  if ((ch&0x7f)<20 && ch!=9 && ch!=0xa && ch!=0xd && ch!=0x85) {
    char buf[6];
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

#include "fmt.h"
#include "textcode.h"
#include "str.h"
#include "haveinline.h"

/* src is UTF-8 encoded */
size_t fmt_jsonescape(char* dest,const char* src,size_t len) {
  register const unsigned char* s=(const unsigned char*) src;
  size_t written=0,i;
  for (i=0; i<len; ++i) {
    switch (s[i]) {
    case '\\':
    case '"':
      if (dest) {
	dest[written]='\\';
	dest[written+1]=s[i];
      }
      written+=2;
      break;
    default:
      if (s[i]<' ') {
	if (dest) {
	  dest[written]='\\';
	  dest[written+1]='u';
	  dest[written+2]='0';
	  dest[written+3]='0';
	  dest[written+4]=fmt_tohex(s[i]>>4);
	  dest[written+5]=fmt_tohex(s[i]&0xf);
	}
	written+=6;
      } else {
	if (dest) dest[written]=s[i];
	++written;
      }
      break;
    }
    /* in case someone gives us malicious input */
    if (written>((size_t)-1)/2) return (size_t)-1;
  }
  return written;
}


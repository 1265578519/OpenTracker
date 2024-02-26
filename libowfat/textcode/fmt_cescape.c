#include "fmt.h"
#include "textcode.h"
#include "str.h"
#include "haveinline.h"

size_t fmt_cescape2(char* dest,const char* src,size_t len,const char* escapeme) {
  register const unsigned char* s=(const unsigned char*) src;
  size_t written=0,i;
  char c;
  for (i=0; i<len; ++i) {
    switch (s[i]) {
    case '\a': c='a'; goto doescape;
    case '\b': c='b'; goto doescape;
    case 0x1b: c='e'; goto doescape;
    case '\f': c='f'; goto doescape;
    case '\n': c='n'; goto doescape;
    case '\r': c='r'; goto doescape;
    case '\t': c='t'; goto doescape;
    case '\v': c='v'; goto doescape;
    case '\\':
	c='\\';
      doescape:
	if (dest) {
	  dest[written]='\\';
	  dest[written+1]=c;
	}
	written+=2;
	break;
    default:
	if (s[i]<' ' || escapeme[str_chr(escapeme,s[i])]==s[i]) {
	  if (dest) {
	    dest[written]='\\';
	    dest[written+1]='x';
	    dest[written+2]=fmt_tohex(s[i]>>4);
	    dest[written+3]=fmt_tohex(s[i]&0xf);
	  }
	  written+=4;
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

size_t fmt_cescape(char* dest,const char* src,size_t len) {
  return fmt_cescape2(dest,src,len,"");
}

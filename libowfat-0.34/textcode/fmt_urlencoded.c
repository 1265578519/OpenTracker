#include "fmt.h"
#include "textcode.h"
#include "str.h"
#include "haveinline.h"

static inline int issafe(unsigned char c) {
  const char safe[] = "$/.=~-_";
  return ((c>='A' && c<='Z') ||
	  (c>='a' && c<='z') ||
	  (c>='0' && c<='9') ||
	  safe[str_chr(safe,c)]);
}

size_t fmt_urlencoded2(char* dest,const char* src,size_t len,const char* escapeme) {
  register const unsigned char* s=(const unsigned char*) src;
  size_t written=0,i;
  for (i=0; i<len; ++i) {
    if (!issafe(s[i]) || escapeme[str_chr(escapeme,s[i])]==s[i]) {
      if (dest) {
	dest[written]='%';
	dest[written+1]=fmt_tohex(s[i]>>4);
	dest[written+2]=fmt_tohex(s[i]&15);
      }
      written+=3;
    } else {
      if (dest) dest[written]=s[i];
      ++written;
    }
    /* in case someone gives us malicious input */
    if (written>((size_t)-1)/2) return (size_t)-1;
  }
  return written;
}

size_t fmt_urlencoded(char* dest,const char* src,size_t len) {
  return fmt_urlencoded2(dest,src,len,"");
}

#include "fmt.h"
#include "textcode.h"
#include "haveinline.h"
#include "str.h"

size_t fmt_ldapescape(char* dest,const char* src,size_t len) {
  register const unsigned char* s=(const unsigned char*) src;
  size_t written=0,i;
  for (i=0; i<len; ++i) {
    if (s[i]=='*' || s[i]=='(' || s[i]==')' || s[i]==0 ||
	s[i]=='\\' || s[i]<' ') {
      if (dest) {
	dest[written]='\\';
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


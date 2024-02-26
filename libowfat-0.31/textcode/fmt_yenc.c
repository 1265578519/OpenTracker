#include "fmt.h"
#include "textcode.h"

size_t fmt_yenc(char* dest,const char* src,size_t len) {
  register const unsigned char* s=(const unsigned char*) src;
  size_t written=0,i;
  int linelen=0;
  for (i=0; i<len; ++i) {
    unsigned char c=s[i]+42;
    switch (c) {
    case ' ':	/* escape space at line ending */
      if (linelen==253) {
	if (dest) dest[written]='\n';
	++written; linelen=0;
      }
      goto dontescape;
    case 'F':	/* escape "^From " */
      if (s[i+1]+42!='r' || s[i+2]+42!='o' || s[i+3]+42!='m' || s[i+4]+42!=' ') goto dontescape;
    case '.':	/* dot at start of line needs to be escaped */
      if (!linelen) goto dontescape;
      /* fall through */
    case 0:
    case '\n':
    case '\r':
    case '=':
      if (dest) dest[written]='=';
      ++written;
      c+=64;
      /* fall through */
    default:
dontescape:
      ++linelen;
      if (dest) dest[written]=c;
      ++written;
      if (linelen>253) {
	if (dest) dest[written]='\n';
	++written; linelen=0;
      }
    }
    /* in case someone gives us malicious input */
    if (written>((size_t)-1)/2) return (size_t)-1;
  }
  if (linelen) {
    if (dest) dest[written]='\n';
    ++written;
  }
  return written;
}

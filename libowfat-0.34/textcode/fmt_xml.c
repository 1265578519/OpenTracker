#include "fmt.h"
#include "textcode.h"
#include "str.h"
#include "haveinline.h"

size_t fmt_xml(char* dest,const char* src,size_t len) {
  register const unsigned char* s=(const unsigned char*) src;
  size_t written=0,i;
  const char* seq;
  for (i=0; i<len; ++i) {
    switch (s[i]) {
    case '&': seq="&amp;"; goto doit;
    case '<': seq="&lt;";
      doit:
	written+=fmt_str(dest?dest+written:0,seq);
	break;
    default: if (dest) dest[written]=s[i]; ++written; break;
    }
    /* in case someone gives us malicious input */
    if (written>((size_t)-1)/2) return (size_t)-1;
  }
  return written;
}

#include "fmt.h"
#include "textcode.h"
#include "scan.h"

size_t scan_ldapescape(const char *src,char *dest,size_t *destlen) {
  register const unsigned char* s=(const unsigned char*) src;
  size_t written=0,i;
  for (i=0; s[i]; ++i) {
    if (s[i]=='\\') {
      unsigned char c;
      int j=scan_fromhex(s[i+1]);
      if (j<0) break;
      c=j<<4;
      j=scan_fromhex(s[i+2]);
      if (j<0) break;
      if (dest) dest[written]=c|j;
      i+=2;
    } else {
      if (dest) dest[written]=s[i];
    }
    ++written;
  }
  if (destlen) *destlen=written;
  return i;
}

#ifdef UNITTEST
#include <assert.h>
#include <string.h>
#undef UNITTEST
#include <scan/scan_fromhex.c>

int main() {
  size_t dl;
  char buf[100];
  assert(scan_ldapescape("test\\n\");",buf,&dl)==4 && dl==4 && !memcmp(buf,"test",4));
  /* check hex escaping */
  assert(scan_ldapescape("test\\0afoo",buf,&dl)==10 && dl==8 && !memcmp(buf,"test\nfoo",8));
  /* check that short sequences are rejected */
  assert(scan_ldapescape("test\\ax",buf,&dl)==4 && dl==4 && !memcmp(buf,"test",4));
}
#endif

#include "fmt.h"
#include "textcode.h"
#include "scan.h"

size_t scan_hexdump(const char *src,char *dest,size_t *destlen) {
  register const unsigned char* s=(const unsigned char*) src;
  size_t written=0,i;
  for (i=0; s[i]; ++i) {
    int j=scan_fromhex(s[i]);
    unsigned char k;
    if (j<0) break;
    k=j<<4;
    j=scan_fromhex(s[i+1]);
    if (j<0) break;
    if (dest) dest[written]=k|j;
    ++i;
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
  char buf[100];
  size_t l;
  memset(buf,0,sizeof(buf));
  assert(scan_hexdump("0123456789abcdef",buf,&l)==16 && l==8 && !memcmp(buf,"\x01\x23\x45\x67\x89\xab\xcd\xef",9));
  memset(buf,'?',sizeof(buf));
  assert(scan_hexdump("0",buf,&l)==0 && l==0 && buf[0]=='?');
}
#endif

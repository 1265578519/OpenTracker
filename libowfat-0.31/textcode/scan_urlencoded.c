#include "fmt.h"
#include "textcode.h"
#include "scan.h"

static size_t inner_scan_urlencoded(const char *src,char *dest,size_t *destlen,int plus) {
  register const unsigned char* s=(const unsigned char*) src;
  size_t written=0,i;
  for (i=0; s[i]; ++i) {
    if (s[i]=='%') {
      int j=scan_fromhex(s[i+1]);
      if (j<0) break;
      dest[written]=j<<4;
      j=scan_fromhex(s[i+2]);
      if (j<0) break;
      dest[written]|=j;
      i+=2;
    } else if (s[i]=='+' && plus)
      dest[written]=' ';
    else
      dest[written]=s[i];
    ++written;
  }
  *destlen=written;
  return i;
}

size_t scan_urlencoded(const char *src,char *dest,size_t *destlen) {
  return inner_scan_urlencoded(src,dest,destlen,1);
}

size_t scan_urlencoded2(const char *src,char *dest,size_t *destlen) {
  return inner_scan_urlencoded(src,dest,destlen,0);
}

#include "fmt.h"
#include "textcode.h"
#include "haveinline.h"

size_t fmt_base64(char* dest,const char* src,size_t len) {
  register const unsigned char* s=(const unsigned char*) src;
  unsigned short bits=0,temp=0;
  size_t written=0,i;
  if (!dest) return (len>((size_t)-1)/2)?(size_t)-1:((len+2)/3)*4;
  for (i=0; i<len; ++i) {
    temp<<=8; temp+=s[i]; bits+=8;
    while (bits>6) {
      dest[written]=base64[((temp>>(bits-6))&63)];
      ++written; bits-=6;
    }
  }
  if (bits) {
    temp<<=(6-bits);
    dest[written]=base64[temp&63];
    ++written;
  }
  while (written&3) { dest[written]='='; ++written; }
  return written;
}

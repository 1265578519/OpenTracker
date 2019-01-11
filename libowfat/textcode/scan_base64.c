#include "textcode.h"
#include "haveinline.h"

static inline int dec(unsigned char x) {
  if (x>='A' && x<='Z') return x-'A';
  if (x>='a' && x<='z') return x-'a'+26;
  if (x>='0' && x<='9') return x-'0'+26+26;
  switch (x) {
  case '+': return 62;
  case '/': return 63;
  default: return -1;
  }
}

size_t scan_base64(const char *src,char *dest,size_t *destlen) {
  unsigned short tmp=0,bits=0;
  register const unsigned char* s=(const unsigned char*) src;
  const char* orig=dest;
  for (;;) {
    int a=dec(*s);
    if (a<0) {
      while (*s=='=' && ((s-(const unsigned char*)src)&3)) ++s;
      break;
    }
    tmp=(tmp<<6)|a; bits+=6;
    ++s;
    if (bits>=8) {
      *dest=(tmp>>(bits-=8));
      ++dest;
    }
  }
  *destlen=dest-orig;
  return (const char*)s-src;
}

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
  const char* orig=src;
  size_t i,j=0;
  for (i=0;;) {
    int a=dec(*s);
    if (a<0) {
      int equal=(*s=='=');
      while (*s=='=' && ((s-(const unsigned char*)src)&3)) ++s;
      tmp &= ((1<<bits)-1);
      if (!tmp || equal) { j=i; orig=(const char*)s; }
      break;
    }
    tmp=(tmp<<6)|a; bits+=6;
    ++s;
    if (bits>=8) {
      bits-=8;
      if (dest) dest[i]=(tmp>>bits);
      ++i;
      if (!bits) { j=i; orig=(const char*)s; }
    }
  }
  if (destlen) *destlen=j;
  return orig-src;
}

#ifdef UNITTEST
#include <assert.h>
#include <string.h>
#include <stdio.h>

int main() {
  char buf[100];
  size_t l;
  memset(buf,0,10); assert(scan_base64("Zm5vcmQ=",buf,&l)==8 && l==5 && !memcmp(buf,"fnord",6));
  /* check that we don't insist on the padding */
  memset(buf,0,10); assert(scan_base64("Zm5vcmQ",buf,&l)==7 && l==5 && !memcmp(buf,"fnord",6));
  /* check the special non-isalnum chars :) */
  memset(buf,0,10); assert(scan_base64("/+8=",buf,&l)==4 && l==2 && !memcmp(buf,"\xff\xef",3));
  return 0;
}
#endif

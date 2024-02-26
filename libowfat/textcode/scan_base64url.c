#include "textcode.h"
#include "haveinline.h"

static inline int dec(unsigned char x) {
  if (x>='A' && x<='Z') return x-'A';
  if (x>='a' && x<='z') return x-'a'+26;
  if (x>='0' && x<='9') return x-'0'+26+26;
  switch (x) {
  case '-': return 62;
  case '_': return 63;
  default: return -1;
  }
}

size_t scan_base64url(const char *src,char *dest,size_t *destlen) {
  unsigned short tmp=0,bits=0;
  register const unsigned char* s=(const unsigned char*) src;
  size_t i;
  for (i=0;;) {
    int a=dec(*s);
    if (a<0) break;	/* base64url does not have padding */
    tmp=(tmp<<6)|a; bits+=6;
    ++s;
    if (bits>=8) {
      bits-=8;
      if (dest) dest[i]=(tmp>>bits);
      ++i;
    }
  }
  if (destlen) *destlen=i;
  return (const char*)s-src;
}

#ifdef UNITTEST
#include <assert.h>
#include <string.h>
#include <stdio.h>

int main() {
  char buf[100];
  size_t l;
  /* check that we don't consume padding */
  memset(buf,0,10); assert(scan_base64url("Zm5vcmQ=",buf,&l)==7 && l==5 && !memcmp(buf,"fnord",6));
  /* check that we don't insist on the padding */
  memset(buf,0,10); assert(scan_base64url("Zm5vcmQ",buf,&l)==7 && l==5 && !memcmp(buf,"fnord",6));
  /* check the special non-isalnum chars :) */
  memset(buf,0,10); assert(scan_base64url("_-8=",buf,&l)==3 && l==2 && !memcmp(buf,"\xff\xef",3));
  return 0;
}
#endif

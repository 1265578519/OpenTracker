#include "fmt.h"
#include "textcode.h"
#include "haveinline.h"

static inline unsigned int enc(unsigned char x) {
  return ((x-1)&077)+'!';
}

size_t fmt_uuencoded(char* dest,const char* src,size_t len) {
  size_t i;
  register const unsigned char* s=(const unsigned char*) src;
  const char* orig=dest;
  size_t tmp;
  if (!dest) return len>((size_t)-1)/2?(size_t)-1:(len+2)/3*4;
  while (len) {
    {
      register unsigned int diff;
      if (len>45) { i=15; diff=45; } else { i=(len+2)/3; diff=len; }
      if (orig) *dest=enc(diff);
      ++dest;
      len-=diff;
    }
    for (; i; --i) {
      tmp=((unsigned long)s[0] << 16) +
	  ((unsigned long)s[1] << 8) +
	  ((unsigned long)s[2]);
      if (orig) {
	dest[0]=enc((tmp>>(3*6))&077);
	dest[1]=enc((tmp>>(2*6))&077);
	dest[2]=enc((tmp>>(1*6))&077);
	dest[3]=enc(tmp&077);
      }
      dest+=4; s+=3;
    }
    if (orig) *dest='\n';
    ++dest;
  }
  return dest-orig;
}

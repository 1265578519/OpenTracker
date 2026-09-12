#include "fmt.h"
#include "textcode.h"
#include "scan.h"

static size_t inner_scan_urlencoded(const char *src,char *dest,size_t *destlen,int plus) {
  register const unsigned char* s=(const unsigned char*) src;
  size_t written=0,i;
  for (i=0; s[i]; ++i) {
    if (s[i]=='%') {
      int j=scan_fromhex(s[i+1]);
      unsigned char c;
      if (j<0) break;
      c=j<<4;
      j=scan_fromhex(s[i+2]);
      if (j<0) break;
      dest[written]=c|j;
      i+=2;
    } else if (s[i]=='+' && plus)
      dest[written]=' ';
    else if (s[i]<=' ' || s[i]=='?' || s[i]=='&')
      break;	/* invalid input */
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

#ifdef UNITTEST
#include <assert.h>
#include <string.h>
#undef UNITTEST
#include <scan/scan_fromhex.c>

int main() {
  char buf[100];
  size_t l;
  /* check base operation */
  memset(buf,'?',sizeof(buf));
  assert(scan_urlencoded("foo%2ebar",buf,&l)==9 && l==7 && !memcmp(buf,"foo.bar?",8));
  /* check + handling */
  memset(buf,'?',sizeof(buf));
  assert(scan_urlencoded("foo+bar",buf,&l)==7 && l==7 && !memcmp(buf,"foo bar?",8));
  assert(scan_urlencoded2("foo+bar",buf,&l)==7 && l==7 && !memcmp(buf,"foo+bar?",8));
  /* check that we abort on invalid sequences */
  memset(buf,'?',sizeof(buf));
  assert(scan_urlencoded("foo%2xbar",buf,&l)==3 && l==3 && !memcmp(buf,"foo?",4));
  assert(scan_urlencoded("foo\nbar",buf,&l)==3 && l==3 && !memcmp(buf,"foo?",4));
  assert(scan_urlencoded("foo bar",buf,&l)==3 && l==3 && !memcmp(buf,"foo?",4));
}
#endif

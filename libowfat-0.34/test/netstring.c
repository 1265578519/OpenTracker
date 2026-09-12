#include "fmt.h"
#include "scan.h"
#include <assert.h>
#include <stdio.h>

int main() {
  char buf[100];
  char* s;
  size_t l;
  const char* orig;
  assert(fmt_netstring(buf,"hello world!",12)==16 && !memcmp(buf,"12:hello world!,",16));
  assert(scan_netstring(buf,16,&s,&l)==16 && s==buf+3 && l==12);

  orig="3:foo,"; assert(scan_netstring(orig,6,&s,&l)==6 && s==orig+2 && l==3);
  orig="4294967295:foo,"; assert(scan_netstring(orig,15,&s,&l)==0);
  orig="18446744073709551615:foo,"; assert(scan_netstring(orig,25,&s,&l)==0);

  assert(fmt_netstring(buf,orig,(size_t)-1)==0);
  assert(fmt_netstring(buf,NULL,(size_t)-1)==0);
}

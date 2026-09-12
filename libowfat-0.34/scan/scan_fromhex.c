#include "scan.h"

int scan_fromhex(unsigned char c) {
  c=(unsigned char)(c-'0');
  if (c<=9) return c;
  c=(unsigned char)(c&~0x20);
  c=(unsigned char)(c-('A'-'0'));
  if (c<6) return c+10;
  return -1;
/* more readable but creates worse code:
  if (c>='0' && c<='9')
    return c-'0';
  else if (c>='A' && c<='F')
    return c-'A'+10;
  else if (c>='a' && c<='f')
    return c-'a'+10;
  return -1;
*/
}

#ifdef UNITTEST
#include <assert.h>
int main() {
  int i;
  for (i=0; i<256; ++i) {
    if (i>='0' && i<='9')
      assert(scan_fromhex(i)==i-'0');
    else if (i>='a' && i<='f')
      assert(scan_fromhex(i)==i-'a'+10);
    else if (i>='A' && i<='F')
      assert(scan_fromhex(i)==i-'A'+10);
    else
      assert(scan_fromhex(i)==-1);
  }
  return 0;
}
#endif

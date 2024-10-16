#include "scan.h"

/* parse a netstring, input buffer is in (len bytes).
 * if parsing is successful:
 *   *dest points to string and *slen is size of string
 *   return number of bytes parsed
 * else
 *   return 0
 * Note: *dest will point inside the input buffer!
 */
size_t scan_netstring(const char* in,size_t len,char** dest,size_t* slen) {
  // [len]":"[string]","
  // 3:foo,3:bar,4:fini,
  unsigned long l;
  size_t n=scan_ulongn(in,len,&l);
  if (!n || /* did not start with a number */
      n+2+l<l || /* overflow */
      n+2+l>len || /* longer than we have input data */
      in[n]!=':' || /* syntax error */
      in[n+l+1]!=',')
    return 0;
  *dest=(char*)in+n+1;
  *slen=l;
  return n+2+l;
}

#ifdef UNITTEST
#include <assert.h>

#undef UNITTEST
#include "scan_ulongn.c"

int main() {
  char buf[]="12:hello world!,";
  char* s;
  size_t l;
  const char* orig;
  assert(scan_netstring(buf,16,&s,&l)==16 && s==buf+3 && l==12);

  orig="3:foo,"; assert(scan_netstring(orig,6,&s,&l)==6 && s==orig+2 && l==3);
  orig="4294967295:foo,"; assert(scan_netstring(orig,15,&s,&l)==0);
  orig="18446744073709551615:foo,"; assert(scan_netstring(orig,25,&s,&l)==0);
}
#endif

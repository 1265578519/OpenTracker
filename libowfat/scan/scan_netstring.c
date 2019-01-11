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

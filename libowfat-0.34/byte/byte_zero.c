#include "byte.h"
#include <string.h>

/* byte_zero sets the bytes out[0], out[1], ..., out[len-1] to 0 */
void byte_zero(void* out, size_t len) {
  /* instead of doing this ourselves, defer to the hopefully amazingly
   * optimized memset from libc */
  memset(out,0,len);
#if 0
  register char* s=out;
  size_t i;
  for (i=0; i<len; ++i)
    s[i]=0;
#endif
}

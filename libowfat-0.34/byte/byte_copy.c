#include <stdint.h>
#include "byte.h"

/* byte_copy copies in[0] to out[0], in[1] to out[1], ... and in[len-1]
 * to out[len-1]. */
void byte_copy(void* out, size_t len, const void* in) {
  char* d=out;
  const char* s=in;
  size_t i;
  for (i=0; i<len; ++i)
    d[i]=s[i];
}

#include "fmt.h"

size_t fmt_utf8(char *dest,uint32_t n) {
  size_t i,j;
  if (n<=0x7f) {
    if (dest) *dest=(char)n;
    return 1;
  }
  for (i=0x3f,j=1; i<=0x7fffffff; i=(i<<5)|0x1f, ++j) {
    if (i>=n) {
      --j;
      if (dest) {
	size_t k=j*6;
	*dest++=(char)(((char)0xc0 >> (j-1)) | (char)(n >> k));
	while (k) {
	  *dest++=(char)(0x80 | ((n >> (k-6)) & 0x3f));
	  k-=6;
	}
      }
      return j+1;
    }
    if (i==0x7fffffff) return 0;
  }
  /* we were asked to encode a value that cannot be encoded */
  return 0;
}

/* unit tested via scan/scan_utf8.c */

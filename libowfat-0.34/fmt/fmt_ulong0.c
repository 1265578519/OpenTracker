#include "fmt.h"

size_t fmt_ulong0(char *dest,unsigned long i,size_t pad) {
  register unsigned int len;
  register unsigned long tmp;
  /* first count the number of bytes needed */
  for (len=1, tmp=i; tmp>9; ++len) tmp/=10;
  /* now see if we need to pad */
  if (dest) {
    while (len<pad) { *dest='0'; ++dest; ++len; }
    fmt_ulong(dest,i);
    return len;
  } else
    return (len<pad?pad:len);
}

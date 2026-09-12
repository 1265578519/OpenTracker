#include "fmt.h"

size_t fmt_ulonglong(char *dest,unsigned long long int i) {
  register unsigned long len;
  unsigned long long tmp,len2;
  /* first count the number of bytes needed */
  for (len=1, tmp=i; tmp>9; ++len) tmp/=10;
  if (dest)
    for (tmp=i, dest+=len, len2=len+1; --len2; tmp/=10)
      *--dest = (char)((tmp%10)+'0');
  return len;
}

#include "fmt.h"

/* "foo" -> "foo  "
 * append padlen-srclen spaces after dest, if that is >= 0.  Truncate
 * only if total length is larger than maxlen.  Return number of
 * characters written. */
size_t fmt_fill(char* dest,size_t srclen,size_t padlen,size_t maxlen) {
  long todo;
  char* olddest=dest;
  char* max=dest+maxlen;
  if ((long)padlen<0 || (long)maxlen<0) return 0;
  if (srclen>maxlen) return maxlen;
  if (dest==0) {
    unsigned long sum=srclen>padlen?srclen:padlen;
    return sum>maxlen?maxlen:sum;
  }
  dest+=srclen;
  for (todo=(long)(padlen-srclen); todo>0; --todo) {
    if (dest>max) break;
    *dest=' '; ++dest;
  }
  return (size_t)(dest-olddest);
}

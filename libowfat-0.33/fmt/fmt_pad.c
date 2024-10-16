#include "fmt.h"

/* "foo" -> "  foo"
 *  write padlen-srclen spaces, if that is >= 0.  Then copy srclen
 *  characters from src.  Truncate only if total length is larger than
 *  maxlen.  Return number of characters written. */
size_t fmt_pad(char* dest,const char* src,size_t srclen,size_t padlen,size_t maxlen) {
  long todo;
  char* olddest=dest;
  char* max=dest+maxlen;
  if ((long)srclen<0 || (long)padlen<0 || (long)maxlen<0) return 0;
  todo=(long)(padlen-srclen);
  if (dest==0) {
    unsigned long sum=srclen>padlen?srclen:padlen;
    return sum>maxlen?maxlen:sum;
  }
  for (; todo>0; --todo) {
    if (dest>max) break;
    *dest=' '; ++dest;
  }
  for (todo=(long)(srclen>maxlen?maxlen:srclen); todo>0; --todo) {
    if (dest>max) break;
    *dest=*src; ++dest; ++src;
  }
  return (size_t)(dest-olddest);
}

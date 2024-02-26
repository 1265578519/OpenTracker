#include "fmt.h"

size_t fmt_long(char *dest,long int i) {
  if (i<0) {
    if (dest) *dest++='-';
    return fmt_ulong(dest,(unsigned long)-i)+1;
  } else
    return fmt_ulong(dest,(unsigned long)i);
}

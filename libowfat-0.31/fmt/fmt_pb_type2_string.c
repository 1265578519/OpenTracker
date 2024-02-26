#include "fmt.h"
#include "rangecheck.h"
#include <string.h>

size_t fmt_pb_type2_string(char* dest,const char* s,size_t l) {
  size_t n=fmt_varint(dest,l);
  if (add_of(n,l,n)) return 0;
  if (dest) memcpy(dest+n-l,s,l);
  return n;
}

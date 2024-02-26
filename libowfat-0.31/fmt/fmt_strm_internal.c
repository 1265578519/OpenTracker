#include <stdarg.h>
#include "fmt.h"

size_t fmt_strm_internal(char* dest, ...) {
  size_t n;
  va_list a;
  const char* s;
  va_start(a,dest);
  for (n=0; (s=va_arg(a,const char*)); ) {
    size_t inc=fmt_str(dest,s);
    if (n+inc<n) return (size_t)-1;
    if (dest) dest+=inc;
    n+=inc;
  }
  va_end(a);
  return n;
}

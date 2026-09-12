#define __LIBOWFAT_INTERNAL
#include "str.h"
#include "buffer.h"

int buffer_puts(buffer* b,const char* x) {
  return buffer_put(b,x,str_len(x));
}

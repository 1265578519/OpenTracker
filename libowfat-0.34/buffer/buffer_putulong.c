#include "buffer.h"
#include "fmt.h"

int buffer_putulong(buffer *b,unsigned long l) {
  char buf[FMT_ULONG];
  return buffer_put(b,buf,fmt_ulong(buf,l));
}


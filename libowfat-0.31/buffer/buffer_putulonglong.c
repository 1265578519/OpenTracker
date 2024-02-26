#include "buffer.h"
#include "fmt.h"

int buffer_putulonglong(buffer *b,unsigned long long l) {
  char buf[FMT_ULONG];
  return buffer_put(b,buf,fmt_ulonglong(buf,l));
}


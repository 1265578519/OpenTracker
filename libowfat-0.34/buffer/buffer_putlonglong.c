#include "buffer.h"
#include "fmt.h"

int buffer_putlonglong(buffer *b,signed long long l) {
  char buf[FMT_LONG];
  return buffer_put(b,buf,fmt_longlong(buf,l));
}


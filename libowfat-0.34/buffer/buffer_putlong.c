#include "buffer.h"
#include "fmt.h"

int buffer_putlong(buffer *b,signed long l) {
  char buf[FMT_LONG];
  return buffer_put(b,buf,fmt_long(buf,l));
}


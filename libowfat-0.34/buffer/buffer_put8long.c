#include "buffer.h"
#include "fmt.h"

int buffer_put8long(buffer *b,unsigned long l) {
  char buf[FMT_8LONG];
  return buffer_put(b,buf,fmt_8long(buf,l));
}


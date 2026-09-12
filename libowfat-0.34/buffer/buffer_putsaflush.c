#include "stralloc.h"
#include "buffer.h"

int buffer_putsaflush(buffer* b,const stralloc* sa) {
  return buffer_putflush(b,sa->s,sa->len);
}

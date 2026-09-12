#include "str.h"
#include "buffer.h"

int buffer_putspace(buffer* b) {
  static char space=' ';
  return buffer_put(b,&space,1);
}

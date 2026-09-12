#include "str.h"
#include "buffer.h"

int buffer_putnlflush(buffer* b) {
  static char nl='\n';
  return buffer_putflush(b,&nl,1);
}

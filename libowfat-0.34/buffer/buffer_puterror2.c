#include "buffer.h"
#include <string.h>

int buffer_puterror2(buffer* b,int errnum) {
  return buffer_puts(b,strerror(errnum));
}

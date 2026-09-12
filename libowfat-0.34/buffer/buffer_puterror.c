#include "buffer.h"
#include <string.h>
#include <errno.h>

int buffer_puterror(buffer* b) {
  return buffer_puts(b,strerror(errno));
}

#include <stdlib.h>
#include "buffer.h"

void buffer_free(void* buf) {
  free(buf);
}

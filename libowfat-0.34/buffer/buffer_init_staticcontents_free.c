#include "buffer.h"
#include <mmap.h>

void buffer_init_staticcontents_free(buffer* b, char* y, size_t len) {
  buffer_init_staticcontents(b, y, len);
  b->deinit=buffer_free;
}

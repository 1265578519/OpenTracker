#include "buffer.h"

char *buffer_peek(buffer* b) {
  return b->x+b->p;
}

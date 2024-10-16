#include "array.h"
#include "buffer.h"

void buffer_fromarray(buffer* b,array* a) {
  if (array_failed(a))
    buffer_frombuf(b,NULL,0);
  else
    buffer_frombuf(b,array_start(a),a->initialized);
}

#include <stdlib.h>
#include "byte.h"
#include "iob_internal.h"

io_batch* iob_new(int hint_entries) {
  io_batch* b=(io_batch*)malloc(sizeof(io_batch));
  if (!b) return 0;
  byte_zero(b,sizeof(io_batch));
  if (hint_entries) {
    if (!array_allocate(&b->b,sizeof(iob_entry),hint_entries)) {
      free(b);
      return 0;
    }
    array_trunc(&b->b);
  }
  return b;
}

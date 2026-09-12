#include "iob_internal.h"

io_batch* iob_new_autofree(size_t hint_entries) {
  io_batch* b=iob_new(hint_entries);
  if (b)
    b->autofree=1;
  return b;
}

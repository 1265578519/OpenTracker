#include "iob_internal.h"

int iob_init_autofree(io_batch* b,size_t hint_entries) {
  int r=iob_init(b,hint_entries);
  b->autofree=1;
  return r;
}

#include <stdlib.h>
#include "iob_internal.h"

void iob_free(io_batch* b) {
  iob_reset(b);
  free(b);
}

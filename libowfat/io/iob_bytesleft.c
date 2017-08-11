#include "iob.h"

uint64 iob_bytesleft(const io_batch* b) {
  return b->bytesleft;
}

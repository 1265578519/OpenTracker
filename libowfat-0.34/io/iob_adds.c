#include <string.h>
#include "iob_internal.h"

int iob_adds(io_batch* b,const char* s) {
  return iob_addbuf(b,s,strlen(s));
}

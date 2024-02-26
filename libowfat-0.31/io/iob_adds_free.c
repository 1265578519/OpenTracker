#include "str.h"
#include "iob.h"

int iob_adds_free(io_batch* b,const char* s) {
  return iob_addbuf_free(b,s,str_len(s));
}

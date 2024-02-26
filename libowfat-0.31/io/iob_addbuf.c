#include "iob_internal.h"

int iob_addbuf(io_batch* b,const void* buf,uint64 n) {
  return iob_addbuf_internal(b,buf,n,0);
}

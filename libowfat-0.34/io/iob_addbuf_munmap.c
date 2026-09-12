#include <sys/types.h>
#include "mmap.h"
#include "iob_internal.h"

static void cleanup(struct iob_entry* x) {
  mmap_unmap((char*)x->buf,x->offset+x->n);
}

int iob_addbuf_munmap(io_batch* b,const void* buf,uint64 n) {
  return iob_addbuf_internal(b,buf,n,cleanup);
}

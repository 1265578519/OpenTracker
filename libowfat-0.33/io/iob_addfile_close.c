#include "iob_internal.h"

static void cleanup(struct iob_entry* x) {
  io_close(x->fd);
}

int iob_addfile_close(io_batch* b,int64 fd,uint64 off,uint64 n) {
  iob_entry* e;
  if (n==0) {
    io_close(fd);
    return 1;
  }
  io_fd(fd);
  e=array_allocate(&b->b,sizeof(iob_entry),
		   array_length(&b->b,sizeof(iob_entry)));
  if (!e) {
    io_close(fd);
    return 0;
  }
  e->type=FROMFILE;
  e->fd=fd;
  e->buf=0;
  e->n=n;
  e->offset=off;
  e->cleanup=cleanup;
  b->bytesleft+=n;
  ++b->files;
  return 1;
}

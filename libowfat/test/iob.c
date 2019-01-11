#include <assert.h>
#include "iob.h"
#include "buffer.h"
#include <unistd.h>

static int64 write_cb(int64 fd,const void* buf,uint64 len) {
  if (len>2) len=2;
  return write(fd,buf,len);
}

int main() {
  int64 fd;
  io_batch* b;
  int64 r;
  assert(io_readfile(&fd,"GNUmakefile"));
  assert(b=iob_new(10));
  assert(iob_addbuf(b,"Huhu",4));
  assert(iob_addbuf(b," fnord\n",7));
  assert(iob_addfile(b,fd,10,10));
  iob_send(1,b);
#if 0
  do {
    r=iob_write(1,b,write_cb);
  } while (r>0);
#endif
  iob_free(b);
  return 0;
}

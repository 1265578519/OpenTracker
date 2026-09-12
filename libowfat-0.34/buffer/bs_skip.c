#include "parse.h"

size_t bs_skip(struct bytestream* bs, size_t len) {
  size_t n;
  if (bs->cur >= bs->max)
    return 0;

  n = bs->max - bs->cur;
  if (len > n) {
    bs->max = 0;	// set error
    bs->cur = 1;
    return 0;
  }

  switch (bs->type) {

  case MEMBUF:
    bs->cur += len;
    break;

  case IOBUF:
    bs->cur += len;
    return buffer_seek(bs->u.b, len);

  case BSTREAM:
    bs->cur += len;
    return bs_skip(bs->u.bs, len);

  }

  return len;
}

#ifdef UNITTEST
#include <assert.h>
#include <signal.h>

#undef UNITTEST
#include "buffer/bs_init_membuf.c"
#include "buffer/bs_err.c"
#include "buffer/bs_get.c"
#include "buffer/buffer_getc.c"
#include "buffer/buffer_seek.c"
#include "buffer/buffer_feed.c"
#include "buffer/buffer_stubborn2.c"
#include "buffer/bs_init_iobuf.c"
#include "open/open_read.c"
#include "buffer/prs_readblob.c"
#include "buffer/buffer_get.c"
#include "buffer/bs_capacityassert.c"
#include "byte/byte_copy.c"
#include "buffer/buffer_init.c"

int main() {
  struct bytestream bs;

  // test basic functionality inside buffer with known length
  bs_init_membuf(&bs, "fnord\n", 7);
  assert(bs_skip(&bs, 0) == 0);
  assert(bs_err(&bs) == 0);

  assert(bs_skip(&bs, 5) == 5);
  assert(bs_err(&bs) == 0);

  assert(bs_get(&bs) == '\n');

  assert(bs_skip(&bs, 100) == 0);
  assert(bs_err(&bs));

  // test basic functionality when the backend is a file
  int fd = open_read("GNUmakefile");
  assert(fd != -1);
  unsigned char iobuf[42];
  buffer b = BUFFER_INIT(read, fd, iobuf+32, 10);
  bs_init_iobuf(&bs, &b);
  assert(bs_skip(&bs, 100) == 100);
  assert(prs_readblob(&bs, iobuf, 10) == 10);
  assert(pread(fd, iobuf+10, 10, 100) == 10);
  assert(!memcmp(iobuf, iobuf+10, 10));

  // test basic functionality when backend is non-seekable file
  int pipes[2];
  assert(pipe(pipes)==0);
  int r=fork();
  assert(r!=-1);
  if (r==0) {
    lseek(fd, 0, SEEK_SET);
    for (;;) {
      r=read(fd, iobuf, 32);
      if (r <= 0) return 0;
      write(pipes[1], iobuf, 32);
    }
  }
  buffer_init(&b, read, pipes[0], iobuf+32, 10);

  bs_init_iobuf(&bs, &b);
  assert(bs_skip(&bs, 20) == 20);
  assert(prs_readblob(&bs, iobuf, 10) == 10);
  assert(pread(fd, iobuf+10, 10, 20) == 10);
  assert(!memcmp(iobuf, iobuf+10, 10));

  kill(r, 15);

  close(fd);
}
#endif

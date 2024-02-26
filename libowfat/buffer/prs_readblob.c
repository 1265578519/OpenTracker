#include "parse.h"

static const size_t max_ssize_t = (((size_t)1) << (sizeof(size_t)*8-1))-1;

/* Read n bytes from stream. Return n.
 * Set stream to error state if not enough space or I/O error. */
ssize_t prs_readblob(struct bytestream* bs,unsigned char* dest,size_t destlen) {
  ssize_t r;
  if (destlen > max_ssize_t)
    destlen = max_ssize_t;
  if (!bs_capacityassert(bs,destlen)) return -1;
  r = destlen;

  switch (bs->type) {

  case MEMBUF:
    memcpy(dest, bs->u.base+bs->cur, destlen);
    bs->cur += destlen;
    break;

  case IOBUF:
    r=buffer_get(bs->u.b, (char*)dest, destlen);
    if (r != (ssize_t)destlen) {
      bs->cur = 1;	// set bytestream to error state
      bs->max = 0;
      return -1;
    }
    bs->cur += r;
    break;

  case BSTREAM:
    r=prs_readblob(bs->u.bs, dest, destlen);
    if (r != (ssize_t)destlen) {
      bs->cur = 1;	// set bytestream to error state
      bs->max = 0;
      return -1;
    }
    bs->cur += r;
    break;

  }

    return r;
}

#ifdef UNITTEST
#include <assert.h>

#undef UNITTEST
#include "buffer/bs_get.c"
#include "buffer/bs_err.c"
#include "buffer/bs_capacityassert.c"
#include "buffer/bs_init_iobuf_size.c"
#include "buffer/bs_init_membuf.c"

#include "buffer/buffer_get.c"
#include "buffer/buffer_getc.c"
#include "buffer/buffer_feed.c"
#include "buffer/buffer_stubborn2.c"
#include "byte/byte_copy.c"
#include "open/open_read.c"

int main() {
  struct bytestream bs = BS_FROM_MEMBUF("\x34\x12", 2);
  unsigned char buf[42];
  assert(prs_readblob(&bs, buf, 2) == 2);
  assert(bs_err(&bs) == 0);
  assert(!memcmp(buf,"\x34\x12",2));

  assert(prs_readblob(&bs, buf, 1) == -1);
  assert(bs_err(&bs));

  // now try to provoke integer overflow
  bs_init_membuf(&bs, (const unsigned char*)"\x34\x12", 2);
  assert(bs_get(&bs) == 0x34);
  assert(prs_readblob(&bs, buf, (size_t)-1) == -1);
  // a bad implementation would add 0xffffffff to the 1 we already read and wrap to 0
  // our code makes sure not to wrap and also limits the length of the
  // blob to max_ssize_t (0x7fffffff).
  assert(bs_err(&bs));

  int fd = open_read("Makefile");
  assert(fd!=-1);
  buffer b = BUFFER_INIT(read, fd, buf+32, 10);
  bs_init_iobuf_size(&bs, &b, 100);
  assert(prs_readblob(&bs, buf, 20) == 20);	// make sure we can read a 20 byte blob at once even if the iobuf buffer size is smaller
  // strace will show two read calls here
}
#endif

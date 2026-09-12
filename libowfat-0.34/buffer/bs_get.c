#include "parse.h"

unsigned char bs_get(struct bytestream* bs) {
  unsigned char r;
  char c;
  if (bs->cur>=bs->max) {	// EOF or already error state?
    bs->max=0;	// signal error
    bs->cur=1;
    return 0;	// return 0
  }
  switch (bs->type) {

  case MEMBUF:
    r=bs->u.base[bs->cur];
    break;

  case IOBUF:
    {
      int ret=buffer_getc(bs->u.b, &c);
      if (ret==1) {
	r=c;
      } else {
	bs->max=0;
	bs->cur=1;
	return 0;
      }
    }
    break;

  case BSTREAM:
    r=bs_get(bs->u.bs);
    break;

  default:
    r=0;	// cannot happen
  }
  ++bs->cur;
  return r;
}

#ifdef UNITTEST
#include <assert.h>

#undef UNITTEST
#include "buffer/bs_err.c"
#include "buffer/buffer_init_staticcontents.c"
#include "buffer/bs_init_iobuf.c"
#include "buffer/bs_init_iobuf_size.c"
#include "buffer/buffer_getc.c"
#include "buffer/buffer_feed.c"
#include "buffer/buffer_stubborn2.c"

int main() {
  struct bytestream bs = BS_FROM_MEMBUF("fnord\nx", 6);
  int i;
  char buf[7];

  /* first test: membuf.
   * See if we get all the bytes we put in and then error is signaled */
  for (i=0; i<6; ++i) {
    buf[i] = bs_get(&bs);
    assert(buf[i] == "fnord\n"[i]);
    assert(!bs_err(&bs));
  }
  buf[6] = bs_get(&bs);
  /* We put an x there in memory.
   * If the bytestream range check failed, we'll get 'x', otherwise 0. */
  assert(buf[6] == 0);
  assert(bs_err(&bs));

  /* second test: iobuf with no limit. Otherwise the same. */

  struct buffer b;
  buffer_init_staticcontents(&b, "fnord\nx", 6);	// this will let us read 6 bytes
  bs_init_iobuf(&bs, &b);
  for (i=0; i<6; ++i) {
    buf[i] = bs_get(&bs);
    assert(buf[i] == "fnord\n"[i]);
    assert(!bs_err(&bs));
  }
  buf[6] = bs_get(&bs);
  /* We put an x there in memory.
   * If the bytestream range check failed, we'll get 'x', otherwise 0. */
  assert(buf[6] == 0);
  assert(bs_err(&bs));

  /* third test: iobuf with limit. Otherwise the same. */
  buffer_init_staticcontents(&b, "fnord\nx", 7);	// this will let us read 7 bytes

  bs_init_iobuf_size(&bs, &b, 6);	// but we tell bytestream the limit is 6
  for (i=0; i<6; ++i) {
    buf[i] = bs_get(&bs);
    assert(buf[i] == "fnord\n"[i]);
    assert(!bs_err(&bs));
  }
  buf[6] = bs_get(&bs);
  /* We put an x there in the backing buffer.
   * If the bytestream range check failed, we'll get 'x', otherwise 0. */
  assert(buf[6] == 0);
  assert(bs_err(&bs));

  /* fourth test: iobuf with EOF */
  buffer_init_staticcontents(&b, "fnord\nx", 6);
  bs_init_iobuf(&bs, &b);	// bytestream has no limit but will hit EOF in backing buffer
  for (i=0; i<6; ++i) {
    buf[i] = bs_get(&bs);
    assert(buf[i] == "fnord\n"[i]);
    assert(!bs_err(&bs));
  }
  buf[6] = bs_get(&bs);
  /* We did not give the bytestream a limit, but the buffer should
   * refuse to return more. */
  assert(buf[6] == 0);
  assert(bs_err(&bs));
}

#endif

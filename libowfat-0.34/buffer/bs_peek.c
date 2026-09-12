#include "parse.h"

unsigned char bs_peek(struct bytestream* bs) {
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
      int ret=buffer_peekc(bs->u.b, &c);
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
    r=bs_peek(bs->u.bs);
    break;

  default:
    r=0;	// cannot happen
  }
  return r;
}

#ifdef UNITTEST
#include <assert.h>

#undef UNITTEST
#include "buffer/bs_err.c"
#include "buffer/bs_get.c"
#include "buffer/buffer_init_staticcontents.c"
#include "buffer/bs_init_iobuf.c"
#include "buffer/bs_init_iobuf_size.c"
#include "buffer/buffer_peekc.c"
#include "buffer/buffer_getc.c"
#include "buffer/buffer_feed.c"
#include "buffer/buffer_stubborn2.c"

int main() {
  struct bytestream bs = BS_FROM_MEMBUF("fx", 1);

  /* first test: membuf.
   * See if we get all the bytes we put in and then error is signaled */
  assert(bs_peek(&bs) == 'f');
  assert(bs_peek(&bs) == 'f');
  assert(!bs_err(&bs));
  assert(bs_get(&bs) == 'f');
  assert(!bs_err(&bs));
  assert(bs_peek(&bs) == 0);
  assert(bs_err(&bs));

  /* second test: iobuf with no limit. Otherwise the same. */
  struct buffer b;
  buffer_init_staticcontents(&b, "fx", 1);
  bs_init_iobuf(&bs, &b);

  assert(bs_peek(&bs) == 'f');
  assert(bs_peek(&bs) == 'f');
  assert(!bs_err(&bs));
  assert(bs_get(&bs) == 'f');
  assert(!bs_err(&bs));
  assert(bs_peek(&bs) == 0);
  assert(bs_err(&bs));

  /* third test: iobuf with limit. Otherwise the same. */
  buffer_init_staticcontents(&b, "fx", 2);
  bs_init_iobuf_size(&bs, &b, 1);

  assert(bs_peek(&bs) == 'f');
  assert(bs_peek(&bs) == 'f');
  assert(!bs_err(&bs));
  assert(bs_get(&bs) == 'f');
  assert(!bs_err(&bs));
  assert(bs_peek(&bs) == 0);
  assert(bs_err(&bs));

  /* fourth test: iobuf with EOF */
  buffer_init_staticcontents(&b, "fx", 1);
  bs_init_iobuf(&bs, &b);	// bytestream has no limit but will hit EOF in backing buffer

  assert(bs_peek(&bs) == 'f');
  assert(bs_peek(&bs) == 'f');
  assert(!bs_err(&bs));
  assert(bs_get(&bs) == 'f');
  assert(!bs_err(&bs));
  assert(bs_peek(&bs) == 0);
  assert(bs_err(&bs));
  return 0;
}

#endif

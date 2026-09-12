#include "parse.h"

void bs_init_iobuf_size(struct bytestream* bs,struct buffer* b,size_t maxlen) {
  bs->type = IOBUF;
  bs->cur = 0;
  bs->max = maxlen;
  bs->u.b=b;
}

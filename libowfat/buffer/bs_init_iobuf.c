#include "parse.h"

void bs_init_iobuf(struct bytestream* bs,struct buffer* b) {
  bs->type = IOBUF;
  bs->cur = 0;
  bs->max = (size_t)-1;
  bs->u.b=b;
}

#include "parse.h"

size_t bs_consumeleftovers(struct bytestream* bs) {
  if (bs->cur >= bs->max)
    return 0;
  return bs_skip(bs, bs->max - bs->cur);
}

/* bs_skip unit tests cover this too */

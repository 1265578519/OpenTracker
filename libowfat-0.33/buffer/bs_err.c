#include "parse.h"

int bs_err(const struct bytestream* bs) {
  return (bs->cur > bs->max);
}


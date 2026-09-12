#define NO_UINT64_MACROS
#include "uint64.h"
#include "uint32.h"

void uint64_pack_big(char *out,uint64 in) {
  uint32_pack_big(out,(uint32)(in>>32));
  uint32_pack_big(out+4,in&0xffffffff);
}

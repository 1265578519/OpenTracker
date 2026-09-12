#define NO_UINT64_MACROS
#include "uint64.h"
#include "uint32.h"

void uint64_pack(char *out,uint64 in) {
  uint32_pack(out,in&0xffffffff);
  uint32_pack(out+4,(uint32)(in>>32));
}

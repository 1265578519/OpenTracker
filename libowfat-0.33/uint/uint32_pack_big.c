#define NO_UINT32_MACROS
#include "uint32.h"

void uint32_pack_big(char *out,uint32 in) {
  out[3]=(char)in; in>>=8;
  out[2]=(char)in; in>>=8;
  out[1]=(char)in; in>>=8;
  out[0]=(char)in;
}

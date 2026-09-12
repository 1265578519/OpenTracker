#define NO_UINT16_MACROS
#include "uint16.h"

void uint16_pack(char *out,uint16 in) {
  out[0]=(char)in;
  out[1]=(char)(in>>8);
}

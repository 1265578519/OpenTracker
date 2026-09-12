#define NO_UINT16_MACROS
#include "uint16.h"

void uint16_unpack_big(const char *in,uint16 *out) {
  *out = (unsigned short)((((unsigned char) in[0]) << 8) + (unsigned char)in[1]);
}

#define NO_UINT16_MACROS
#include "uint16.h"

uint16 uint16_read_big(const char *in) {
  return (unsigned short)((((unsigned char) in[0]) << 8) + (unsigned char)in[1]);
}

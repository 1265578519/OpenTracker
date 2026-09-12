#define NO_UINT16_MACROS
#include "uint16.h"

uint16 uint16_read(const char *in) {
  return (unsigned short)((((unsigned char) in[1]) << 8) | (unsigned char)in[0]);
}

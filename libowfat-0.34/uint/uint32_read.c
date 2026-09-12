#define NO_UINT32_MACROS
#include "uint32.h"

uint32 uint32_read(const char *in) {
  return (((uint32)(unsigned char)in[3])<<24) |
         (((uint32)(unsigned char)in[2])<<16) |
         (((uint32)(unsigned char)in[1])<<8) |
          (uint32)(unsigned char)in[0];
}

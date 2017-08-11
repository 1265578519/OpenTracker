#define NO_UINT32_MACROS
#include "uint32.h"

void uint32_pack(char *out,uint32 in) {
  *out=(char)in; in>>=8;
  *++out=(char)in; in>>=8;
  *++out=(char)in; in>>=8;
  *++out=(char)in;
}

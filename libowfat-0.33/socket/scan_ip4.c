#include "scan.h"
#include "ip4.h"

size_t scan_ip4(const char *s,char ip[4])
{
  size_t i,len;
  unsigned long u;

  len = 0;
  for (i=0; i<4; ++i) {
    register size_t j;
    len+=(j=scan_ulong(s,&u))+1;
    if (!j || u>255) return 0;
    ip[i]=(char)u; s+=j;
    if (i<3 && *s!='.') return 0;
    ++s;
  }
  return len-1;
}

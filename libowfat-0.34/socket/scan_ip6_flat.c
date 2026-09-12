#include "scan.h"

size_t scan_ip6_flat(const char *s,char ip[16])
{
  size_t i;
  for (i=0; i<16; i++) {
    int tmp;
    tmp=scan_fromhex((unsigned char)*s++);
    if (tmp<0) return 0;
    ip[i]=(char)(tmp << 4);
    tmp=scan_fromhex((unsigned char)*s++);
    if (tmp<0) return 0;
    ip[i] = ip[i] | (char)tmp;
  }
  return 32;
}

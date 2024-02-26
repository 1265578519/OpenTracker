#include "fmt.h"
#include "ip4.h"

size_t fmt_ip4(char *s,const char ip[4])
{
  size_t i,len;

  len = 0;
  for (i=0; i<4; ++i) {
    register size_t j;
    len+=(j=fmt_ulong(s,(unsigned long) (unsigned char) ip[i]))+1;
    if (s && i<3) { s+=j; *s++='.'; }
  }
  return len-1;
}

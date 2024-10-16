#include "byte.h"
#include "fmt.h"
#include "dns.h"

void dns_name4_domain(char name[DNS_NAME4_DOMAIN],const char ip[4])
{
  unsigned int namelen;
  unsigned int i;

  namelen = 0;
  i = fmt_ulong(name + namelen + 1,(unsigned long) (unsigned char) ip[3]);
  name[namelen++] = i;
  namelen += i;
  i = fmt_ulong(name + namelen + 1,(unsigned long) (unsigned char) ip[2]);
  name[namelen++] = i;
  namelen += i;
  i = fmt_ulong(name + namelen + 1,(unsigned long) (unsigned char) ip[1]);
  name[namelen++] = i;
  namelen += i;
  i = fmt_ulong(name + namelen + 1,(unsigned long) (unsigned char) ip[0]);
  name[namelen++] = i;
  namelen += i;
  byte_copy(name + namelen,14,"\7in-addr\4arpa\0");
}

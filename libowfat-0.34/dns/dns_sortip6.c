#include "byte.h"
#include "dns.h"

/* XXX: sort servers by configurable notion of closeness? */
/* XXX: pay attention to competence of each server? */

void dns_sortip6(char *s,unsigned int n)
{
  unsigned int i;
  char tmp[16];

  n >>= 4;
  while (n > 1) {
    i = dns_random(n);
    --n;
    byte_copy(tmp,16,s + (i << 4));
    byte_copy(s + (i << 4),16,s + (n << 4));
    byte_copy(s + (n << 4),16,tmp);
  }
}

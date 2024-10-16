#include "byte.h"
#include "fmt.h"
#include "dns.h"
#include "haveinline.h"

/* RFC1886:
 *   4321:0:1:2:3:4:567:89ab
 * ->
 *   b.a.9.8.7.6.5.0.4.0.0.0.3.0.0.0.2.0.0.0.1.0.0.0.0.0.0.0.1.2.3.4.IP6.INT.
 *   b.a.9.8.7.6.5.0.4.0.0.0.3.0.0.0.2.0.0.0.1.0.0.0.0.0.0.0.1.2.3.4.IP6.ARPA.
 */

static char tohex(char c) {
  return c>=10?c-10+'a':c+'0';
}

void dns_name6_domain(char name[DNS_NAME6_DOMAIN],const char ip[16])
{
  unsigned int j;

  for (j=0; j<16; j++) {
    name[j*4]=1;
    name[j*4+1]=tohex(ip[15-j] & 15);
    name[j*4+2]=1;
    name[j*4+3]=tohex((unsigned char)ip[15-j] >> 4);
  }
  byte_copy(name + 4*16,10,"\3ip6\4arpa\0");
}


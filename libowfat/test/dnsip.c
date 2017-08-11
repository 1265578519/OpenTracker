#include <dns.h>
#include <ip4.h>
#include "buffer.h"
#include <errno.h>
#include <string.h>
#include "errmsg.h"

int main(int argc,char* argv[]) {
  static char seed[128];
  static stralloc fqdn;
  static stralloc out;
  char str[IP4_FMT];
  unsigned int i;

  errmsg_iam("dnsip");
  (void)argc;

  dns_random_init(seed);
  if (*argv) ++argv;
  while (*argv) {
    if (!stralloc_copys(&fqdn,*argv)) {
      buffer_putsflush(buffer_2,"out of memory\n");
      return 111;
    }
    if (dns_ip4(&out,&fqdn) == -1)
      diesys(111,"unable to find IP address for ",*argv);

    for (i = 0;i + 4 <= out.len;i += 4) {
      if (i) buffer_puts(buffer_1," ");
      buffer_put(buffer_1,str,ip4_fmt(str,out.s + i));
    }
    buffer_puts(buffer_1,"\n");
    ++argv;
  }
  buffer_flush(buffer_1);
  return 0;
}

#include "stralloc.h"
#include "uint16.h"
#include "byte.h"
#include "dns.h"

int dns_ip4_packet(stralloc *out,const char *buf,unsigned int len)
{
  unsigned int pos;
  char header[12];
  uint16 numanswers;
  uint16 datalen;

  if (!stralloc_copys(out,"")) return -1;

  pos = dns_packet_copy(buf,len,0,header,12); if (!pos) return -1;
  uint16_unpack_big(header + 6,&numanswers);
  pos = dns_packet_skipname(buf,len,pos); if (!pos) return -1;
  pos += 4;

  while (numanswers--) {
    pos = dns_packet_skipname(buf,len,pos); if (!pos) return -1;
    pos = dns_packet_copy(buf,len,pos,header,10); if (!pos) return -1;
    uint16_unpack_big(header + 8,&datalen);
    if (byte_equal(header,2,DNS_T_A))
      if (byte_equal(header + 2,2,DNS_C_IN))
        if (datalen == 4) {
	  if (!dns_packet_copy(buf,len,pos,header,4)) return -1;
	  if (!stralloc_catb(out,header,4)) return -1;
	}
    pos += datalen;
  }

  dns_sortip(out->s,out->len);
  return 0;
}

static char *q = 0;

int dns_ip4(stralloc *out,const stralloc *fqdn)
{
  unsigned int i;
  char code;
  char ch;

  if (!stralloc_copys(out,"")) return -1;
  code = 0;
  for (i = 0;i <= fqdn->len;++i) {
    if (i < fqdn->len)
      ch = fqdn->s[i];
    else
      ch = '.';

    if ((ch == '[') || (ch == ']')) continue;
    if (ch == '.') {
      if (!stralloc_append(out,&code)) return -1;
      code = 0;
      continue;
    }
    if ((ch >= '0') && (ch <= '9')) {
      code *= 10;
      code += ch - '0';
      continue;
    }

    if (!dns_domain_fromdot(&q,fqdn->s,fqdn->len)) return -1;
    if (dns_resolve(q,DNS_T_A) == -1) return -1;
    if (dns_ip4_packet(out,dns_resolve_tx.packet,dns_resolve_tx.packetlen) == -1) return -1;
    dns_transmit_free(&dns_resolve_tx);
    dns_domain_free(&q);
    return 0;
  }

  out->len &= ~3;
  return 0;
}

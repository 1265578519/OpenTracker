#include "stralloc.h"
#include "byte.h"
#include "uint16.h"
#include "dns.h"

static char *q = 0;

int dns_mx_packet(stralloc *out,const char *buf,unsigned int len)
{
  unsigned int pos;
  char header[12];
  char pref[2];
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
    if (byte_equal(header,2,DNS_T_MX))
      if (byte_equal(header + 2,2,DNS_C_IN)) {
	if (!dns_packet_copy(buf,len,pos,pref,2)) return -1;
	if (!dns_packet_getname(buf,len,pos + 2,&q)) return -1;
	if (!stralloc_catb(out,pref,2)) return -1;
	if (!dns_domain_todot_cat(out,q)) return -1;
	if (!stralloc_0(out)) return -1;
      }
    pos += datalen;
  }

  return 0;
}

int dns_mx(stralloc *out,const stralloc *fqdn)
{
  if (!dns_domain_fromdot(&q,fqdn->s,fqdn->len)) return -1;
  if (dns_resolve(q,DNS_T_MX) == -1) return -1;
  if (dns_mx_packet(out,dns_resolve_tx.packet,dns_resolve_tx.packetlen) == -1) return -1;
  dns_transmit_free(&dns_resolve_tx);
  dns_domain_free(&q);
  return 0;
}

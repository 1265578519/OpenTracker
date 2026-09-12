#include "stralloc.h"
#include "uint16.h"
#include "byte.h"
#include "dns.h"
#include "ip4.h"
#include "ip6.h"

static int dns_ip6_packet_add(stralloc *out,const char *buf,unsigned int len)
{
  unsigned int pos;
  char header[16];
  uint16 numanswers;
  uint16 datalen;

  pos = dns_packet_copy(buf,len,0,header,12); if (!pos) return -1;
  uint16_unpack_big(header + 6,&numanswers);
  pos = dns_packet_skipname(buf,len,pos); if (!pos) return -1;
  pos += 4;

  while (numanswers--) {
    pos = dns_packet_skipname(buf,len,pos); if (!pos) return -1;
    pos = dns_packet_copy(buf,len,pos,header,10); if (!pos) return -1;
    uint16_unpack_big(header + 8,&datalen);
    if (byte_equal(header,2,DNS_T_AAAA)) {
      if (byte_equal(header + 2,2,DNS_C_IN))
        if (datalen == 16) {
	  if (!dns_packet_copy(buf,len,pos,header,16)) return -1;
	  if (!stralloc_catb(out,header,16)) return -1;
	}
    } else if (byte_equal(header,2,DNS_T_A))
      if (byte_equal(header + 2,2,DNS_C_IN))
        if (datalen == 4) {
	  byte_copy(header,12,V4mappedprefix);
	  if (!dns_packet_copy(buf,len,pos,header+12,4)) return -1;
	  if (!stralloc_catb(out,header,16)) return -1;
	}
    pos += datalen;
  }

  dns_sortip6(out->s,out->len);
  return 0;
}

int dns_ip6_packet(stralloc *out,const char *buf,unsigned int len) {
  if (!stralloc_copys(out,"")) return -1;
  return dns_ip6_packet_add(out,buf,len);
}

static char *q = 0;

int dns_ip6(stralloc *out,stralloc *fqdn)
{
  unsigned int i;
  char code;
  char ch;
  char ip[16];

  if (!stralloc_copys(out,"")) return -1;
  if (!stralloc_readyplus(fqdn,1)) return -1;
  fqdn->s[fqdn->len]=0;
  if ((i=scan_ip6(fqdn->s,ip))) {
    if (fqdn->s[i]) return -1;
    stralloc_copyb(out,ip,16);
    return 0;
  }
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
    if (!stralloc_copys(out,"")) return -1;
    if (dns_resolve(q,DNS_T_AAAA) != -1)
      if (dns_ip6_packet_add(out,dns_resolve_tx.packet,dns_resolve_tx.packetlen) != -1) {
	dns_transmit_free(&dns_resolve_tx);
	dns_domain_free(&q);
      }
    if (!dns_domain_fromdot(&q,fqdn->s,fqdn->len)) return -1;
    if (dns_resolve(q,DNS_T_A) != -1)
      if (dns_ip6_packet_add(out,dns_resolve_tx.packet,dns_resolve_tx.packetlen) != -1) {
	dns_transmit_free(&dns_resolve_tx);
	dns_domain_free(&q);
      }
    return out->a>0?0:-1;
  }

  out->len &= ~3;
  return 0;
}

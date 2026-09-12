#include "scan.h"
#include "ip4.h"
#include "ip6.h"

/*
 * IPv6 addresses are really ugly to parse.
 * Syntax: (h = hex digit)
 *   1. hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh
 *   2. any number of 0000 may be abbreviated as "::", but only once
 *   3. The last two words may be written as IPv4 address
 */

size_t scan_ip6(const char *s,char ip[16])
{
  size_t i;
  size_t len=0;
  unsigned long u;

  char suffix[16];
  unsigned int prefixlen=0;
  unsigned int suffixlen=0;

  if ((i=scan_ip4(s,ip+12))) {
    for (len=0; len<12; ++len) ip[len]=V4mappedprefix[len];
    return i;
  }
  for (i=0; i<16; i++) ip[i]=0;
  for (;;) {
    if (*s == ':') {
      ++len;
      ++s;
      if (*s == ':') {	/* Found "::", skip to part 2 */
	++len;
	++s;
	break;
      }
    }
    i = scan_xlong(s,&u);
    if (!i) return 0;
    if (prefixlen==12 && s[i]=='.') {
      /* the last 4 bytes may be written as IPv4 address */
      i=scan_ip4(s,ip+12);
      if (i)
	return i+len;
      else
	return 0;
    }
    ip[prefixlen++] = (char)(u >> 8);
    ip[prefixlen++] = (char)u;
    s += i; len += i;
    if (prefixlen==16)
      return len;
  }

/* part 2, after "::" */
  for (;;) {
    if (*s == ':') {
      if (suffixlen==0)
	break;
      s++;
      len++;
    } else if (suffixlen)
      break;
    i = scan_xlong(s,&u);
    if (!i) {
      if (suffixlen)
	--len;
      break;
    }
    if (suffixlen+prefixlen<=12 && s[i]=='.') {
      size_t j=scan_ip4(s,suffix+suffixlen);
      if (j) {
	suffixlen+=4;
	len+=j;
	break;
      } else
	prefixlen=12-suffixlen;	/* make end-of-loop test true */
    }
    suffix[suffixlen++] = (char)(u >> 8);
    suffix[suffixlen++] = (char)u;
    s += i; len += i;
    if (prefixlen+suffixlen==16)
      break;
  }
  for (i=0; i<suffixlen; i++)
    ip[16-suffixlen+i] = suffix[i];
  return len;
}

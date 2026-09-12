/*
DNS should have used LZ77 instead of its own sophomoric compression algorithm.
*/

#include <errno.h>
#include "dns.h"

unsigned int dns_packet_copy(const char *buf,unsigned int len,unsigned int pos,char *out,unsigned int outlen)
{
  while (outlen) {
    if (pos >= len) { errno = EINVAL; return 0; }
    *out = buf[pos++];
    ++out; --outlen;
  }
  return pos;
}

unsigned int dns_packet_skipname(const char *buf,unsigned int len,unsigned int pos)
{
  unsigned char ch;

  for (;;) {
    if (pos >= len) break;
    ch = buf[pos++];
    if (ch >= 192) return pos + 1;
    if (ch >= 64) break;
    if (!ch) return pos;
    pos += ch;
  }

  errno = EINVAL;
  return 0;
}

unsigned int dns_packet_getname(const char *buf,unsigned int len,unsigned int pos,char **d)
{
  unsigned int loop = 0;
  unsigned int state = 0;
  unsigned int firstcompress = 0;
  unsigned int where;
  unsigned char ch;
  char name[255];
  unsigned int namelen = 0;

  for (;;) {
    if (pos >= len) goto PROTO;
    ch = buf[pos++];
    if (++loop >= 1000) goto PROTO;

    if (state) {
      if (namelen + 1 > sizeof name) goto PROTO;
      name[namelen++] = ch;
      --state;
    }
    else {
      while (ch >= 192) {
	where = ch; where -= 192; where <<= 8;
	if (pos >= len) goto PROTO;
	ch = buf[pos++];
	if (!firstcompress) firstcompress = pos;
	pos = where + ch;
	if (pos >= len) goto PROTO;
	ch = buf[pos++];
	if (++loop >= 1000) goto PROTO;
      }
      if (ch >= 64) goto PROTO;
      if (namelen + 1 > sizeof name) goto PROTO;
      name[namelen++] = ch;
      if (!ch) break;
      state = ch;
    }
  }

  if (!dns_domain_copy(d,name)) return 0;

  if (firstcompress) return firstcompress;
  return pos;

  PROTO:
  errno = EINVAL;
  return 0;
}

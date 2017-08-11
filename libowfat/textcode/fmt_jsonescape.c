#include "fmt.h"
#include "textcode.h"
#include "str.h"
#include "scan.h"
#include "haveinline.h"

/* src is UTF-8 encoded */
size_t fmt_jsonescape(char* dest,const char* src,size_t len) {
  register const unsigned char* s=(const unsigned char*) src;
  size_t written=0,i;
  char c;
  for (i=0; i<len; ++i) {
    switch (s[i]) {
    case '\\':
    case '"':
      c=s[i];
escape:
      if (dest) {
	dest[written]='\\';
	dest[written+1]=c;
      }
      written+=2;
      break;
    case '\n': c='n'; goto escape;
    case '\r': c='r'; goto escape;
    case '\b': c='b'; goto escape;
    case '\t': c='t'; goto escape;
    case '\f': c='f'; goto escape;
    default:
      if (s[i]<' ') {
	if (dest) {
	  dest[written]='\\';
	  dest[written+1]='u';
	  dest[written+2]='0';
	  dest[written+3]='0';
	  dest[written+4]=fmt_tohex(s[i]>>4);
	  dest[written+5]=fmt_tohex(s[i]&0xf);
	}
	written+=6;
      } else if (s[i]>0x7f) {
	/* UTF-8! Convert to surrogate pair if needed. */
	uint32_t u;
	size_t j=scan_utf8_sem((const char*)s+i,len-i,&u);
	if (j==0) /* Invalid UTF-8! Abort! */
	  return written;
	if (u>0xffff) {
	  if (dest) {
	    dest[written  ]='\\';
	    dest[written+1]='u';
	    fmt_xlong(dest+written+2,0xd800 + ((u>>10) & 0x3bf));
	    dest[written+6]='\\';
	    dest[written+7]='u';
	    fmt_xlong(dest+written+8,0xdc00 + (u & 0x3ff));
	  }
	  written+=12;
	} else {
	  if (dest) memcpy(dest+written,s+i,j);
	  written+=j;
	}
	i+=j-1;	/* -1 because the for loop will also add 1 */
	break;
      } else {
	if (dest) dest[written]=s[i];
	++written;
      }
      break;
    }
    /* in case someone gives us malicious input */
    if (written>((size_t)-1)/2) return (size_t)-1;
  }
  return written;
}

#ifdef UNITTEST
#include <assert.h>
#include <string.h>

int main() {
  char buf[100];
  /* test utf-8 pass-through and correct encoding of \t */
  assert(fmt_jsonescape(buf,"\tfnörd",7)==8 && !memcmp(buf,"\\tfnörd",8));
  /* test escaping of unprintable characters */
  assert(fmt_jsonescape(buf,"\001x",2)==7 && !memcmp(buf,"\\u0001x",7));
  /* test conversion of large UTF-8 chars to UTF-16 surrogate pairs (poop emoji) */
  assert(fmt_jsonescape(buf,"\xf0\x9f\x92\xa9x",5)==13 && !memcmp(buf,"\\ud83d\\udca9x",13));
}
#endif

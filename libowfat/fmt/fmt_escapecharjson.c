#include "fmt.h"

#ifdef UNITTEST
#undef UNITTEST
#include "fmt_tohex.c"
#define UNITTEST
#endif

static void fmt_hex4(char* dest,uint16_t w) {
  dest[3]=fmt_tohex(w&0xf); w>>=4;
  dest[2]=fmt_tohex(w&0xf); w>>=4;
  dest[1]=fmt_tohex(w&0xf); w>>=4;
  dest[0]=fmt_tohex(w&0xf);
}

size_t fmt_escapecharjson(char* dest,uint32_t ch) {
  size_t n;
  switch (ch) {
  case '\b':
    ch='b'; goto simple;
  case '\n':
    ch='n'; goto simple;
  case '\r':
    ch='r'; goto simple;
  case '"':
  case '\\':
  case '/':
simple:
    if (dest) {
      dest[0]='\\';
      dest[1]=(char)ch;
    }
    return 2;
  }
  if (ch>0xffff) {
    if (ch>0x10ffff) return 0;	// highest representable unicode codepoint
    if (dest) {
      dest[0]='\\';
      dest[1]='u';
      fmt_hex4(dest+2,(uint16_t)(0xd800 | (((ch-0x10000)>>10)&0x3ff)));
      dest+=6;
    }
    ch=(ch&0x3ff)|0xdc00;
    n=6;
  } else
    n=0;
  if (dest) {
    dest[0]='\\';
    dest[1]='u';
    fmt_hex4(dest+2,(uint16_t)ch);
  }
  return n+6;
}

#ifdef UNITTEST
#include <assert.h>
#include <string.h>

int main() {
  char buf[100];
  assert(fmt_escapecharjson(buf,'f')==6 && !memcmp(buf,"\\u0066",6));
  assert(fmt_escapecharjson(buf,'\b')==2 && !memcmp(buf,"\\b",2));
  assert(fmt_escapecharjson(buf,'\n')==2 && !memcmp(buf,"\\n",2));
  assert(fmt_escapecharjson(buf,'\r')==2 && !memcmp(buf,"\\r",2));
  assert(fmt_escapecharjson(buf,'"')==2 && !memcmp(buf,"\\\"",2));
  assert(fmt_escapecharjson(buf,'\\')==2 && !memcmp(buf,"\\\\",2));
  assert(fmt_escapecharjson(buf,'/')==2 && !memcmp(buf,"\\/",2));	/* I'm baffled as well */
  assert(fmt_escapecharjson(buf,0x1d11e)==12 && !memcmp(buf,"\\ud834\\udd1e",12));	/* utf-16 surrogate pairs */
  return 0;
}
#endif

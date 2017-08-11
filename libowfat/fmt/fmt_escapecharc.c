#include "fmt.h"

#ifdef UNITTEST
#include "fmt_tohex.c"
#endif

static void fmt_oct3(char* dest,uint8_t w) {
  dest[2]=(char)((w&7)+'0'); w>>=3;
  dest[1]=(char)((w&7)+'0'); w>>=3;
  dest[0]=(char)((w&7)+'0');
}

size_t fmt_escapecharc(char* dest,uint32_t ch) {
  char c;
  if (ch>0xff) {
    if (dest) {
      unsigned int i;
      dest[0]='\\';
      if (ch>0xffff) {
	dest[1]='U';
	dest[2]=fmt_tohex((ch>>28)&0xf);
	dest[3]=fmt_tohex((ch>>24)&0xf);
	dest[4]=fmt_tohex((ch>>20)&0xf);
	dest[5]=fmt_tohex((ch>>16)&0xf);
	i=6;
      } else {
	dest[1]='u';
	i=2;
      }
      dest[i]=fmt_tohex((ch>>12)&0xf);
      dest[i+1]=fmt_tohex((ch>>8)&0xf);
      dest[i+2]=fmt_tohex((ch>>4)&0xf);
      dest[i+3]=fmt_tohex(ch&0xf);
    }
    return (ch>0xffff ? 10 : 6);
  }
  switch (ch) {
  case '\a': c='a'; goto doescape;
  case '\b': c='b'; goto doescape;
  case 0x1b: c='e'; goto doescape;
  case '\f': c='f'; goto doescape;
  case '\n': c='n'; goto doescape;
  case '\r': c='r'; goto doescape;
  case '\t': c='t'; goto doescape;
  case '\v': c='v'; goto doescape;
  case '\\':
    c='\\';
  doescape:
    if (dest) {
      dest[0]='\\';
      dest[1]=c;
    }
    return 2;
  default:
    if (dest) {
      dest[0]='\\';
      fmt_oct3(dest+1,ch&0xff);
    }
    return 4;
  }
}

#ifdef UNITTEST
#include <assert.h>
#include <string.h>
#include <stdio.h>

int main() {
  char buf[100];
  assert(fmt_escapecharc(buf,0)==4 && !memcmp(buf,"\\000",4));
  assert(fmt_escapecharc(buf,'\a')==2 && !memcmp(buf,"\\a",2));
  assert(fmt_escapecharc(buf,'\b')==2 && !memcmp(buf,"\\b",2));
  assert(fmt_escapecharc(buf,'\f')==2 && !memcmp(buf,"\\f",2));
  assert(fmt_escapecharc(buf,'\n')==2 && !memcmp(buf,"\\n",2));
  assert(fmt_escapecharc(buf,'\r')==2 && !memcmp(buf,"\\r",2));
  assert(fmt_escapecharc(buf,0x1b)==2 && !memcmp(buf,"\\e",2));
  assert(fmt_escapecharc(buf,'\t')==2 && !memcmp(buf,"\\t",2));
  assert(fmt_escapecharc(buf,'\v')==2 && !memcmp(buf,"\\v",2));
  assert(fmt_escapecharc(buf,'\\')==2 && !memcmp(buf,"\\\\",2));
  assert(fmt_escapecharc(buf,'1')==4 && !memcmp(buf,"\\001",2));
  assert(fmt_escapecharc(buf,0xfefe)==6 && !memcmp(buf,"\\ufefe",6));
  assert(fmt_escapecharc(buf,0xfefec0de)==10 && !memcmp(buf,"\\Ufefec0de",10));
  return 0;
}

#endif

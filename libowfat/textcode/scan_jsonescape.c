#include "fmt.h"
#include "textcode.h"
#include "scan.h"

size_t scan_jsonescape(const char *src,char *dest,size_t *destlen) {
  register const unsigned char* s=(const unsigned char*) src;
  size_t written=0,i;
  char c;
  unsigned int prev,todo;
  prev=(unsigned int)-1;
  for (i=0; s[i]; ++i) {
    if (s[i]=='"') {
      if (prev!=(unsigned int)-1) goto abort;
      goto done;
    }
    if ((c=s[i])=='\\') {
      switch (s[i+1]) {
      case '\\':
	// c='\\';	// c already is backslash
	break;
      case '"': c='"'; break;
      case 'n': c='\n'; break;
      case 'r': c='\r'; break;
      case 'b': c='\b'; break;
      case 'f': c='\f'; break;
      case 't': c='\t'; break;
      case 'u':
	{
	  size_t j;
	  unsigned int cur;
	  for (cur=j=0; j<4; ++j) {
	    char x=scan_fromhex(s[i+2+j]);
	    if (x<0) goto abort;	// not hex -> invalid input
	    cur=(cur<<4) | x;
	  }
	  if (cur>=0xd800 && cur<=0xdbff) {
	    // utf-16 surrogate pair; needs to be followed by another
	    // surrogate. We need to read both and convert to UTF-8
	    if (prev!=(unsigned int)-1) goto abort;	// two lead surrogates
	    prev=cur;
	    i+=5;	// we want i to go up by 6, 1 is done by the for loop
	    continue;	// write nothing!
	  } else if (cur>=0xdc00 && cur<=0xdfff) {
	    if (prev==(unsigned int)-1) goto abort;	// no lead surrogate
	    todo=(cur&0x3ff) | ((prev&0x3ff) << 10) | 0x10000;
	  } else
	    todo=cur;
	  written+=fmt_utf8(dest?dest+written:dest,todo);
	  i+=5;
	  prev=-1;
	  continue;
	}
      default:
	c=s[i+1];
	break;
      }
      ++i;
    }
    if (prev!=(unsigned int)-1) goto abort;
    /* We expect utf-8 incoming. Make sure it's valid. */
    if (!scan_utf8(src+i,4,NULL)) goto abort;
    if (dest) dest[written]=c;
    ++written;
  }
done:
  *destlen=written;
  return i;
abort:
  if (prev!=(unsigned int)-1) i-=6; // if we abort and there still was an open surrogate pair, cancel it
  *destlen=written;
  return i;
}

#ifdef UNITTEST
#include <assert.h>
#include <string.h>
#undef UNITTEST
#include <scan/scan_fromhex.c>
#include <scan/scan_utf8.c>
#include <fmt/fmt_utf8.c>

int main() {
  char buf[100];
  size_t l;
  assert(scan_jsonescape("fnord",buf,&l)==5 && l==5 && !memcmp(buf,"fnord",5));
  /* is \n properly unescaped? */
  assert(scan_jsonescape("a\\nb",buf,&l)==4 && l==3 && !memcmp(buf,"a\nb",3));
  assert(scan_jsonescape("a\\rb",buf,&l)==4 && l==3 && !memcmp(buf,"a\rb",3));
  assert(scan_jsonescape("a\\bb",buf,&l)==4 && l==3 && !memcmp(buf,"a\bb",3));
  assert(scan_jsonescape("a\\fb",buf,&l)==4 && l==3 && !memcmp(buf,"a\fb",3));
  assert(scan_jsonescape("a\\tb",buf,&l)==4 && l==3 && !memcmp(buf,"a\tb",3));
  assert(scan_jsonescape("a\\\\b",buf,&l)==4 && l==3 && !memcmp(buf,"a\\b",3));
  assert(scan_jsonescape("a\\/b",buf,&l)==4 && l==3 && !memcmp(buf,"a/b",3));
  assert(scan_jsonescape("a\\\"b",buf,&l)==4 && l==3 && !memcmp(buf,"a\"b",3));
  /* does a double quote end the string? */
  assert(scan_jsonescape("a\"b",buf,&l)==1 && l==1 && !memcmp(buf,"a",1));
  /* how about unicode escape */
  assert(scan_jsonescape("a\\u005cb",buf,&l)==8 && l==3 && !memcmp(buf,"a\\b",3));
  /* a trailing surrogate pair with no lead before it */
  assert(scan_jsonescape("a\\udead\"",buf,&l)==1);
  /* a lead surrogate pair with no trailer behind it */
  assert(scan_jsonescape("a\\udafd\"",buf,&l)==1);
  assert(scan_jsonescape("a\\udafd\\udafd",buf,&l)==1);
  assert(scan_jsonescape("a\\udafd0",buf,&l)==1);
  /* correct surrogate pair */
  assert(scan_jsonescape("a\\ud834\\udd1eb",buf,&l)==14 && l==6 && !memcmp(buf,"a\xf0\x9d\x84\x9e""b",6));
  assert(scan_jsonescape("\\ud83d\\udca9x",buf,&l)==13 && l==5 && !memcmp(buf,"\xf0\x9f\x92\xa9x",5));
  /* how about some incorrect UTF-8? */
  assert(scan_jsonescape("a\xc0\xaf",buf,&l)==1 && l==1 && !memcmp(buf,"a",1));
  return 0;
}
#endif



#include "fmt.h"
#include "textcode.h"
#include "scan.h"

size_t scan_jsonescape(const char *src,char *dest,size_t *destlen) {
  register const unsigned char* s=(const unsigned char*) src;
  size_t written=0,i;
  char c;
  unsigned int prev,cur,todo;
  prev=cur=(unsigned int)-1;
  for (i=0; s[i]; ++i) {
    if ((c=s[i])=='\\') {
      switch (s[i+1]) {
      case '\\':
	if (prev!=(unsigned int)-1) return 0; // lead surrogate not followed by tail surrogate
	// c='\\';	// c already is backslash
	break;
      case 'u':
	{
	  size_t j;
	  for (cur=j=0; j<4; ++j) {
	    char x=scan_fromhex(s[i+2+j]);
	    if (x<0) return 0;	// not hex -> invalid input
	    cur=(cur<<4) | x;
	  }
	  if (cur>=0xd800 && cur<=0xdbff) {
	    // utf-16 surrogate pair; needs to be followed by another
	    // surrogate. We need to read both and convert to UTF-8
	    if (prev!=(unsigned int)-1) return 0;	// two lead surrogates
	    prev=cur;
	    i+=5;	// we want i to go up by 6, 1 is done by the for loop
	    continue;	// write nothing!
	  } else if (cur>=0xdc00 && cur<=0xdfff) {
	    todo=(cur&0x3ff) | ((prev&0x3ff) << 10) | 0x100000;
	  } else
	    todo=cur;
	  written+=fmt_utf8(dest?dest+written:dest,todo);
	  i+=5;
	  prev=-1;
	  continue;
	}
      default:
	if (prev!=(unsigned int)-1) return 0; // lead surrogate not followed by tail surrogate
	c=s[i+1];
	break;
      }
      ++i;
    }
    if (dest) dest[written]=c;
    ++written;
  }
  *destlen=written;
  return i;
}

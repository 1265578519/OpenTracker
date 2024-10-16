#include "fmt.h"
#include "textcode.h"
#include "scan.h"

size_t scan_cescape(const char *src,char *dest,size_t *destlen) {
  register const unsigned char* s=(const unsigned char*) src;
  size_t written=0,i;
  char c;
  for (i=0; s[i]; ++i) {
    if ((c=s[i])=='\\') {
      switch (s[i+1]) {
      case 'a': c='\a'; break;
      case 'b': c='\b'; break;
      case 'e': c=0x1b; break;
      case 'f': c='\f'; break;
      case 'n': c='\n'; break;
      case 'r': c='\r'; break;
      case 't': c='\t'; break;
      case 'v': c='\v';
      case '\\': break;
      case 'x':	// hex escape; \x0a -> 10
	{
	  unsigned char a,b;
	  a=scan_fromhex(s[i+2]);
	  b=scan_fromhex(s[i+3]);
	  if (a<16) {
	    if (b<16) {
	      c=(a<<4)+b;
	      i+=2;
	    } else {
	      c=a;
	      i+=1;
	    }
	  }
	}
	break;
      case 'u': // C99 unicode escape: \u000a -> 10
      case 'U': // C99 unicode escape: \U0000000a -> 10
	{
	  unsigned int j,k=0,l=(s[i+1]=='U'?10:6);
	  for (j=2; j<l; ++j) {
	    unsigned char c=scan_fromhex(s[i+j]);
	    if (c>=16) // error
	      goto error;	// don't allow short sequences
	    k=k*16+c;
	  }
	  written+=fmt_utf8(dest?dest+written:0,k);
	  i+=j-1;
	  continue;
	}
      default:
	if (s[i+1]>='0' && s[i+1]<='7') {	// octal escape; \012 -> 10
	  unsigned int j,k;
	  for (k=0,j=1; j<4; ++j) {
	    unsigned int l=s[i+j]-'0';
	    if (l<8)
	      k=k*8+l;
	    else
	      break;
	  }
	  if (dest) dest[written++]=k;
	  i+=j-1;
	  continue;
	}
	--i;
      }
      ++i;
    } else if (c=='"')
      break;
    if (dest) dest[written]=c;
    ++written;
  }
error:
  if (destlen) *destlen=written;
  return i;
}

#ifdef UNITTEST
#include <assert.h>
#include <string.h>
#undef UNITTEST
#include <scan/scan_fromhex.c>
#include <fmt/fmt_utf8.c>

int main() {
  size_t dl;
  char buf[100];
  assert(scan_cescape("test\\n\");",buf,&dl)==6 && dl==5 && !memcmp(buf,"test\n",5));
  /* check hex and octal escaping */
  assert(scan_cescape("test\\x0a\");",buf,&dl)==8 && dl==5 && !memcmp(buf,"test\n",5));
  assert(scan_cescape("test\\012\");",buf,&dl)==8 && dl==5 && !memcmp(buf,"test\n",5));
  /* check short escape sequences */
  assert(scan_cescape("test\\xa\");",buf,&dl)==7 && dl==5 && !memcmp(buf,"test\n",5));
  assert(scan_cescape("test\\12\");",buf,&dl)==7 && dl==5 && !memcmp(buf,"test\n",5));
  assert(scan_cescape("test\\1\");",buf,&dl)==6 && dl==5 && !memcmp(buf,"test\1",5));
  /* check unicode */
  assert(scan_cescape("test\\u000a\");",buf,&dl)==10 && dl==5 && !memcmp(buf,"test\n",5));
  assert(scan_cescape("test\\U0000000a\");",buf,&dl)==14 && dl==5 && !memcmp(buf,"test\n",5));
  /* check that short sequences are rejected */
  assert(scan_cescape("test\\Ua\");",buf,&dl)==4 && dl==4 && !memcmp(buf,"test",4));
}
#endif

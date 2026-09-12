#include "array.h"
#include "textcode.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <write12.h>

array a;

int main() {
  char buf[256];
  int i;
  for (i=0; i<256; ++i) buf[i]=i;

  fmt_to_array(fmt_uuencoded,&a,buf,256);
  assert(!array_failed(&a));
  write(1,array_start(&a),array_bytes(&a));
  array_trunc(&a);

  fmt_to_array(fmt_base64,&a,buf,256);
  assert(!array_failed(&a));
  write(1,array_start(&a),array_bytes(&a)); write(1,"\n",1);
  array_trunc(&a);

  strcpy(buf,"&Auml;cht fn&ouml;rdig.");
  size_t n;
  scan_html(buf,buf,&n);
  buf[n]=0;
  puts(buf);

  assert(fmt_base64(buf,"fnörd",6)==8 && !memcmp(buf,"Zm7DtnJk",8));
  assert(scan_base64("Zm7DtnJk",buf,&n)==8 && n==6 && !memcmp(buf,"fnörd",6));
  assert(fmt_base64(buf,"x",1)==4 && !memcmp(buf,"eA==",4));
  assert(scan_base64("eA====",buf,&n)==4 && n==1 && buf[0]=='x');
  assert(fmt_base64url(buf,"\xef\xbf\xbdx",4)==6 && !memcmp(buf,"77-9eA",6));
  assert(scan_base64url("77-9eA",buf,&n)==6 && n==4 && !memcmp(buf,"\xef\xbf\xbdx",4));
  return 0;
}

#include "fmt.h"
#include "str.h"
#include <assert.h>
#include "scan.h"
#include <byte.h>

int main() {
  char buf[1024];
  long long l;

  assert(fmt_longlong(0,12345)==5);
  assert(fmt_longlong(0,-12345)==6);
  assert(fmt_longlong(buf,12345)==5); buf[5]=0;
  assert(str_equal(buf,"12345"));
  assert(scan_longlong(buf,&l)==5); assert(l==12345);
  assert(fmt_longlong(buf,-12345)==6); buf[6]=0;
  assert(str_equal(buf,"-12345"));
  assert(scan_longlong(buf,&l)==6); assert(l==-12345);

  assert(fmt_longlong(0,1234567890)==10);
  assert(fmt_longlong(0,-1234567890)==11);
  assert(fmt_longlong(buf,1234567890)==10); buf[10]=0;
  assert(str_equal(buf,"1234567890"));
  assert(scan_longlong(buf,&l)==10); assert(l==1234567890);

  assert(fmt_longlong(buf,-1234567890)==11); buf[11]=0;
  assert(str_equal(buf,"-1234567890"));
  assert(scan_longlong(buf,&l)==11); assert(l==-1234567890);

  assert(fmt_xlonglong(buf,0x8000000000000000)==16 && byte_equal(buf,16,"8000000000000000"));
  return 0;
}

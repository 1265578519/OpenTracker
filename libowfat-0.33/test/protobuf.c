#include "fmt.h"
#include "scan.h"
#include <math.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

int main() {
  char buf[100];
  double pi=M_PI;
  float fpi=pi;

  unsigned long long l;
  size_t len;
  unsigned char c;

  assert(fmt_varint(buf,23)==1 && buf[0]==23);
  assert(scan_varint(buf,1,&l)==1 && l==23);
  assert(fmt_varint(buf,150)==2 && buf[0]==(char)0x96 && buf[1]==1);
  assert(scan_varint(buf,2,&l)==2 && l==150);

  assert(fmt_pb_tag(buf,23,2)==2 && (unsigned char)buf[0]==((23<<3)+2));
  assert(scan_pb_tag(buf,2,&len,&c)==2 && len==23 && c==2);

  assert(fmt_pb_type0_int(buf,150)==2 && scan_pb_type0_int(buf,2,&l)==2 && l==150);
  assert(fmt_pb_type0_sint(buf,150)==2 && scan_pb_type0_sint(buf,2,&l)==2 && l==150);

  {
    double d;
    assert(fmt_pb_type1_double(buf,pi)==8 && scan_pb_type1_double(buf,8,&d)==8 && d==pi);
  }

  {
    float f;
    assert(fmt_pb_type5_float(buf,fpi)==4 && scan_pb_type5_float(buf,4,&f)==4 && f==fpi);
  }

  {
    const char* s;
    size_t l;
    assert(fmt_pb_type2_string(buf,"fnord",5)==6);
    assert(scan_pb_type2_stringlen(buf,6,&s,&l)==1 && l==5);
  }

  assert(fmt_pb_int(buf,1,150)==3 && !memcmp(buf,"\x08\x96\x01",3));
  assert(fmt_pb_sint(buf,2,150)==3 && !memcmp(buf,"\x10\xac\x02",3));
  assert(fmt_pb_double(buf,3,pi)==9 && buf[0]==0x19);
  assert(fmt_pb_float(buf,4,fpi)==5 && buf[0]==0x25);
  assert(fmt_pb_string(buf,5,"fnord",5)==7 && !memcmp(buf,"\x2a\x05""fnord",7));
}

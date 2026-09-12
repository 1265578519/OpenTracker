#include <assert.h>

#include <stdio.h>
#include <stdlib.h>

#include "fmt.h"
#include "stralloc.h"
#include "buffer.h"
#include "scan.h"
#include <textcode.h>
#include <byte.h>
#include <uint16.h>
#include <uint32.h>
#include <uint64.h>
#include <openreadclose.h>
#include <mmap.h>
#include "str.h"

char buf[100];
stralloc sa;

void zap() { size_t i; for (i=0; i<sizeof(buf); ++i) buf[i]='_'; }

void zap2() { size_t i; for (i=0; i<sizeof(buf); ++i) buf[i]=i; }

int main() {
  uint32_t x;

  unsigned long long ull;
  unsigned long ul;
  unsigned int ui;
  unsigned short us;
  signed long long ll;
  signed long l;
  signed int i;
  signed short s;

  long flen;
  char* stdiocopy;

#ifdef NDEBUG
#error This is a unit test that uses assert() or all checks, compile without -DNDEBUG!
#endif

  // check utf8 encoding
  zap(); assert(fmt_utf8(NULL,12345) == 3);
  zap(); assert(fmt_utf8(buf,12345) == 3 && byte_equal(buf,4,"\xe3\x80\xb9_"));
  zap(); assert(fmt_utf8(buf,0xa9) == 2 && byte_equal(buf,3,"\xc2\xa9_"));
  zap(); assert(fmt_utf8(buf,0)==1 && byte_equal(buf,2,"\x00_"));
  zap(); assert(fmt_utf8(buf,0x80000000)==0 && buf[0]=='_');

  // check utf8 decoding
  assert(scan_utf8("",1,&x)==1 && x==0);	// one byte
  assert(scan_utf8("\xf0",1,&x)==0);		// incomplete sequence
  assert(scan_utf8("\xc2\xa9",2,&x)==2 && x==0xa9);
  assert(scan_utf8("\xe3\x80\xb9",3,&x)==3 && x==12345);
  assert(scan_utf8("\xc0\xaf",2,&x)==0);	// '/' in non-minimal encoding
  assert(scan_utf8("\xc2\x00",2,&x)==0);	// second byte does not start with 10 bits

  // check ascii encoding of numbers
  assert(fmt_long(NULL,12345)==5);
  zap(); assert(fmt_long(buf,12345)==5 && byte_equal(buf,6,"12345_"));
  assert(fmt_long(NULL,-12345)==6);
  zap(); assert(fmt_long(buf,-12345)==6 && byte_equal(buf,7,"-12345_"));
  assert(fmt_ulong(NULL,123456)==6);
  zap(); assert(fmt_ulong(buf,123456)==6 && byte_equal(buf,7,"123456_"));
  assert(fmt_xlong(NULL,0x12345)==5);
  zap(); assert(fmt_xlong(buf,0x12345)==5 && byte_equal(buf,6,"12345_"));
  assert(fmt_8long(NULL,012345)==5);
  zap(); assert(fmt_8long(buf,012345)==5 && byte_equal(buf,6,"12345_"));
  assert(fmt_longlong(NULL,12345678900ll)==11);
  zap(); assert(fmt_longlong(buf,12345678900ll)==11 && byte_equal(buf,12,"12345678900_"));
  assert(fmt_longlong(NULL,-12345678900ll)==12);
  zap(); assert(fmt_longlong(buf,-12345678900ll)==12 && byte_equal(buf,13,"-12345678900_"));
  assert(fmt_xlonglong(NULL,12345678900ll)==9);
  zap(); assert(fmt_xlonglong(buf,12345678900ll)==9 && byte_equal(buf,10,"2dfdc1c34_"));
  assert(fmt_ulonglong(NULL,12345678900ull)==11);
  zap(); assert(fmt_ulonglong(buf,12345678900ull)==11 && byte_equal(buf,12,"12345678900_"));

  assert(fmt_ulong0(NULL,12345,7)==7);
  zap(); assert(fmt_ulong0(buf,12345,7)==7 && byte_equal(buf,8,"0012345_"));
  assert(fmt_plusminus(NULL,15)==1);
  zap(); assert(fmt_plusminus(buf,15)==1 && byte_equal(buf,2,"+_"));
  assert(fmt_plusminus(NULL,-15)==1);
  zap(); assert(fmt_plusminus(buf,-15)==1 && byte_equal(buf,2,"-_"));
  assert(fmt_plusminus(NULL,0)==0);
  zap(); assert(fmt_plusminus(buf,0)==0 && buf[0]=='_');
  assert(fmt_minus(NULL,15)==0);
  zap(); assert(fmt_minus(buf,15)==0 && buf[0]=='_');
  assert(fmt_minus(NULL,-15)==1);
  zap(); assert(fmt_minus(buf,-15)==1 && byte_equal(buf,2,"-_"));
  assert(fmt_minus(NULL,0)==0);
  zap(); assert(fmt_minus(buf,0)==0 && buf[0]=='_');

  assert(fmt_str(NULL,"blah")==4);
  zap(); assert(fmt_str(buf,"blah")==4 && byte_equal(buf,5,"blah_"));
  zap(); assert(fmt_strn(buf,"blah",3)==3 && byte_equal(buf,4,"bla_"));
  zap(); assert(fmt_strn(buf,"blah",5)==4 && byte_equal(buf,5,"blah_"));

  assert(fmt_pad(NULL,"blah",3,5,7)==5);
  zap(); assert(fmt_pad(buf,"blah",3,5,7)==5 && byte_equal(buf,6,"  bla_"));
  zap(); assert(fmt_pad(buf,"blah",3,5,5)==5 && byte_equal(buf,6,"  bla_"));
  zap(); assert(fmt_pad(buf,"blah",4,5,5)==5 && byte_equal(buf,6," blah_"));
  zap(); assert(fmt_pad(buf,"blah",4,3,5)==4 && byte_equal(buf,5,"blah_"));
  zap(); assert(fmt_pad(buf,"blah",4,3,3)==3 && byte_equal(buf,4,"bla_"));

  assert(fmt_fill(NULL,3,5,7)==5);
  zap(); fmt_str(buf,"bla"); assert(fmt_fill(buf,3,5,7)==5 && byte_equal(buf,6,"bla  _"));
  zap(); fmt_str(buf,"bla"); assert(fmt_fill(buf,3,5,5)==5 && byte_equal(buf,6,"bla  _"));
  zap(); fmt_str(buf,"blah"); assert(fmt_fill(buf,4,5,5)==5 && byte_equal(buf,6,"blah _"));
  zap(); fmt_str(buf,"blah"); assert(fmt_fill(buf,4,3,5)==4 && byte_equal(buf,5,"blah_"));
  zap(); fmt_strn(buf,"blah",3); assert(fmt_fill(buf,4,3,3)==3 && byte_equal(buf,4,"bla_"));

  assert(fmt_human(NULL,12345)==5);
  zap(); assert(fmt_human(buf,12345)==5 && byte_equal(buf,6,"12.3k_"));
  zap(); assert(fmt_human(buf,1024)==4 && byte_equal(buf,5,"1.0k_"));
  zap(); assert(fmt_human(buf,1000)==4 && byte_equal(buf,5,"1.0k_"));
  assert(fmt_humank(NULL,12345)==5);
  zap(); assert(fmt_humank(buf,12345)==5 && byte_equal(buf,6,"12.1k_"));
  zap(); assert(fmt_humank(buf,1024)==4 && byte_equal(buf,5,"1.0k_"));
  zap(); assert(fmt_humank(buf,1000)==4 && byte_equal(buf,5,"1.0k_"));

  zap(); assert(fmt_httpdate(buf,1393353927)==29 && byte_equal(buf,30,"Tue, 25 Feb 2014 18:45:27 GMT_"));

  assert(fmt_asn1derlength(NULL,0)==1);
  zap(); assert(fmt_asn1derlength(buf,0)==1 && byte_equal(buf,2,"\x00_"));
  assert(fmt_asn1derlength(NULL,0xc2)==2);
  zap(); assert(fmt_asn1derlength(buf,0xc2)==2 && byte_equal(buf,3,"\x81\xc2_"));
  assert(fmt_asn1derlength(NULL,0x1234)==3);
  zap(); assert(fmt_asn1derlength(buf,0x1234)==3 && byte_equal(buf,3,"\x82\x12\x34_"));

  assert(fmt_asn1dertag(NULL,0)==1);
  zap(); assert(fmt_asn1dertag(buf,0)==1 && byte_equal(buf,2,"\x00_"));
  assert(fmt_asn1dertag(NULL,0xc2)==2);
  zap(); assert(fmt_asn1dertag(buf,0xc2)==2 && byte_equal(buf,3,"\x81\x42_"));

  ull=-1; assert(scan_asn1dertag("\x00_",2,&ull)==1 && ull==0);
  ull=-1; assert(scan_asn1dertag("\x81\x42_",3,&ull)==2 && ull==0xc2);
  ull=-1; assert(scan_asn1dertag("\x80\x42_",3,&ull)==0 && ull==-1);	// non-minimal encoding
  ull=-1; assert(scan_asn1dertag("\x80_",1,&ull)==0 && ull==-1);	// incomplete sequence
  ull=-1; assert(scan_asn1dertag("\x82\x80_",2,&ull)==0 && ull==-1);	// incomplete sequence
  ull=-1; assert(scan_asn1dertag("\xff\xff\xff\xff\xff\xff\xff\xff\xff\x7f",10,&ull)==0 && ull==-1);	// value too large

  ull=-1; assert(scan_asn1derlength("\x00_",2,&ull)==1 && ull==0);
  ull=-1; assert(scan_asn1derlengthvalue("\x81\xc2_",3,&ull)==2 && ull==0xc2);
  ull=-1; assert(scan_asn1derlengthvalue("\x82\x12\x34_",2,&ull)==0 && ull==-1);
  ull=-1; assert(scan_asn1derlengthvalue("\x82\x12\x34_",4,&ull)==3 && ull==0x1234);
  ull=-1; assert(scan_asn1derlengthvalue("\x82\x00\x34_",4,&ull)==0 && ull==-1);	// non-minimal encoding
  ull=-1; assert(scan_asn1derlengthvalue("\x81\x12_",3,&ull)==0 && ull==-1);	// non-minimal encoding
  ull=-1; assert(scan_asn1derlengthvalue("\xff_",1,&ull)==0 && ull==-1);	// incomplete sequence
  ull=-1; assert(scan_asn1derlengthvalue("\xff_",200,&ull)==0 && ull==-1);	// incomplete sequence
  ull=-1; assert(scan_asn1derlengthvalue("\x89\x12\x23\x34\x45\x56\x67\x78\x89_",200,&ull)==0 && ull==-1);	// value too large

  ull=-1; assert(scan_asn1derlength("\x10_",1,&ull)==0 && ull==-1);	// not enough space in buffer for length

  zap(); assert(fmt_strm(buf,"hell","o, worl","d!\n")==14 && byte_equal(buf,15,"hello, world!\n_"));

  assert(fmt_escapecharxml(NULL,0xc2)==6);
  zap(); assert(fmt_escapecharxml(buf,0xc2)==6 && byte_equal(buf,7,"&#194;_"));
  assert(fmt_escapecharxml(NULL,0)==4);
  zap(); assert(fmt_escapecharxml(buf,0)==4 && byte_equal(buf,5,"&#0;_"));

  assert(fmt_escapecharjson(NULL,'\\')==2);	// "\\"
  zap(); assert(fmt_escapecharjson(buf,'\\')==2 && byte_equal(buf,3,"\\\\_"));
  assert(fmt_escapecharjson(NULL,0xc2)==6);	// "\u00c2"
  zap(); assert(fmt_escapecharjson(buf,0xc2)==6 && byte_equal(buf,7,"\\u00c2_"));
  assert(fmt_escapecharjson(NULL,0x1d11e)==12);	// "\ud834\xdd1e"
  zap(); assert(fmt_escapecharjson(buf,0x1d11e)==12 && byte_equal(buf,13,"\\ud834\\udd1e_"));

  assert(fmt_escapecharquotedprintable(NULL,'=')==3);	// =3d
  zap(); assert(fmt_escapecharquotedprintable(buf,'=')==3 && byte_equal(buf,4,"=3d_"));
  assert(fmt_escapecharquotedprintable(NULL,0xf6)==3);	// =f6
  zap(); assert(fmt_escapecharquotedprintable(buf,0xf6)==3 && byte_equal(buf,4,"=f6_"));
  assert(fmt_escapecharquotedprintable(NULL,0x100)==0);

  assert(fmt_escapecharquotedprintableutf8(NULL,'=')==3);	// =3d
  zap(); assert(fmt_escapecharquotedprintableutf8(buf,'=')==3 && byte_equal(buf,4,"=3d_"));
  assert(fmt_escapecharquotedprintableutf8(NULL,0xf6)==6);	// =c3=b6
  zap(); assert(fmt_escapecharquotedprintableutf8(buf,0xf6)==6 && byte_equal(buf,7,"=c3=b6_"));

  assert(scan_ulong("23.",&ul)==2 && ul==23);
  assert(scan_uint("23.",&ui)==2 && ui==23);
  assert(scan_ushort("23.",&us)==2 && us==23);
  assert(scan_ulonglong("23.",&ull)==2 && ull==23);

  assert(scan_long("23.",&l)==2 && l==23);
  assert(scan_int("23.",&i)==2 && i==23);
  assert(scan_short("23.",&s)==2 && s==23);
  assert(scan_longlong("23.",&ll)==2 && ll==23);

  assert(scan_8long("23.",&l)==2 && l==023);
  assert(scan_8int("23.",&i)==2 && i==023);
  assert(scan_8short("23.",&s)==2 && s==023);
  assert(scan_8longlong("23.",&ll)==2 && ll==023);

  assert(scan_xlong("23.",&l)==2 && l==0x23);
  assert(scan_xint("23.",&i)==2 && i==0x23);
  assert(scan_xshort("23.",&s)==2 && s==0x23);
  assert(scan_xlonglong("23.",&ll)==2 && ll==0x23);

  assert(scan_ulong("46halbe",&ul)==2 && ul==46);
  if (sizeof(ul)==4) {
    assert(scan_ulong("4294967295",&ul)==10 && ul==0xffffffff);
    assert(scan_ulong("4294967296",&ul)==9 && ul==429496729);
    assert(scan_xlong("ffffffff",&ul)==8 && ul==0xffffffff);
    assert(scan_xlong("ffffffff0",&ul)==8 && ul==0xffffffff);
    assert(scan_8long("37777777777",&ul)==11 && ul==0xffffffff);
    assert(scan_8long("377777777771",&ul)==11 && ul==0xffffffff);

    assert(scan_long("2147483647",&l)==10 && l==0x7fffffff);
    assert(scan_long("02147483647",&l)==11 && l==0x7fffffff);
    assert(scan_long("021474836470",&l)==11 && l==0x7fffffff);
    assert(scan_long("+2147483647",&l)==11 && l==0x7fffffff);
    assert(scan_long("+2147483648",&l)==10 && l==214748364);
    assert(scan_long("-2147483647",&l)==11 && l==-2147483647);
    assert(scan_long("-2147483648",&l)==11 && l==-2147483648);
    assert(scan_long("-2147483649",&l)==10 && l==-214748364);

  } else {
    assert(scan_ulong("18446744073709551615",&ul)==20 && ul==0xffffffffffffffffull);
    assert(scan_ulong("18446744073709551616",&ul)==19 && ul==1844674407370955161ull);
    assert(scan_xlong("ffffffffffffffff",&ul)==16 && ul==0xffffffffffffffffull);
    assert(scan_xlong("ffffffffffffffff0",&ul)==16 && ul==0xffffffffffffffffull);
    assert(scan_8long("1777777777777777777777",&ul)==22 && ul==0xffffffffffffffffull);
    assert(scan_8long("17777777777777777777770",&ul)==22 && ul==0xffffffffffffffffull);

    assert(scan_long("9223372036854775807",&l)==19 && l==0x7fffffffffffffffll);
    assert(scan_long("09223372036854775807",&l)==20 && l==0x7fffffffffffffffll);
    assert(scan_long("092233720368547758070",&l)==20 && l==0x7fffffffffffffffll);
    assert(scan_long("+9223372036854775807",&l)==20 && l==0x7fffffffffffffffll);
    assert(scan_long("+9223372036854775808",&l)==19 && l==922337203685477580ll);
    assert(scan_long("-9223372036854775807",&l)==20 && l==-9223372036854775807ll);
    assert(scan_long("-9223372036854775808",&l)==20 && l==(signed long long)0x8000000000000000ull);
    assert(scan_long("-9223372036854775809",&l)==19 && l==-922337203685477580ll);

  }
  assert(scan_uint("4294967295",&ui)==10 && ui==0xffffffff);
  assert(scan_uint("4294967296",&ui)==9 && ui==429496729);
  assert(scan_ushort("65535",&us)==5 && us==0xffff);
  assert(scan_ushort("65536",&us)==4 && us==6553);
  assert(scan_ushort("655350",&us)==5 && us==0xffff);
  assert(scan_ushort("75535",&us)==4 && us==7553);


  assert(scan_xint("0ffffffff",&ui)==9 && ui==0xffffffff);
  assert(scan_xint("ffffffff0",&ui)==8 && ui==0xffffffff);
  assert(scan_xshort("0ffff",&us)==5 && us==0xffff);
  assert(scan_xshort("ffff1",&us)==4 && us==0xffff);

  assert(scan_8int("037777777777",&ui)==12 && ui==0xffffffff);
  assert(scan_8int("377777777771",&ui)==11 && ui==0xffffffff);
  assert(scan_8short("00177777",&us)==8 && us==0xffff);
  assert(scan_8short("1777771",&us)==6 && us==0xffff);

  assert(scan_int("2147483647",&i)==10 && i==0x7fffffff);
  assert(scan_int("02147483647",&i)==11 && i==0x7fffffff);
  assert(scan_int("021474836470",&i)==11 && i==0x7fffffff);
  assert(scan_int("+2147483647",&i)==11 && i==0x7fffffff);
  assert(scan_int("+2147483648",&i)==10 && i==214748364);
  assert(scan_int("-2147483647",&i)==11 && i==-2147483647);
  assert(scan_int("-2147483648",&i)==11 && i==-2147483648);
  assert(scan_int("-2147483649",&i)==10 && i==-214748364);

  assert(scan_short("32767",&s)==5 && s==32767);
  assert(scan_short("32768",&s)==4 && s==3276);
  assert(scan_short("+032767",&s)==7 && s==32767);
  assert(scan_short("-32768",&s)==6 && s==-32768);
  assert(scan_short("-032769",&s)==6 && s==-3276);

  assert(scan_ulonglong("18446744073709551615",&ull)==20 && ull==0xffffffffffffffffull);
  assert(scan_ulonglong("18446744073709551616",&ull)==19 && ull==1844674407370955161ull);
  assert(scan_xlonglong("ffffffffffffffff",&ull)==16 && ull==0xffffffffffffffffull);
  assert(scan_xlonglong("ffffffffffffffff0",&ull)==16 && ull==0xffffffffffffffffull);
  assert(scan_8longlong("1777777777777777777777",&ull)==22 && ull==0xffffffffffffffffull);
  assert(scan_8longlong("17777777777777777777770",&ull)==22 && ull==0xffffffffffffffffull);

  assert(scan_longlong("9223372036854775807",&ll)==19 && ll==0x7fffffffffffffffll);
  assert(scan_longlong("09223372036854775807",&ll)==20 && ll==0x7fffffffffffffffll);
  assert(scan_longlong("092233720368547758070",&ll)==20 && ll==0x7fffffffffffffffll);
  assert(scan_longlong("+9223372036854775807",&ll)==20 && ll==0x7fffffffffffffffll);
  assert(scan_longlong("+9223372036854775808",&ll)==19 && ll==922337203685477580ll);
  assert(scan_longlong("-9223372036854775807",&ll)==20 && ll==-9223372036854775807ll);
  assert(scan_longlong("-9223372036854775808",&ll)==20 && ll==(signed long long)0x8000000000000000ull);
  assert(scan_longlong("-9223372036854775809",&ll)==19 && ll==-922337203685477580ll);

  zap2();

  assert(sizeof(uint16) == sizeof(uint16_t));
  assert(sizeof(uint32) == sizeof(uint32_t));
  assert(sizeof(uint64) == sizeof(uint64_t));
  assert(sizeof(int16) == sizeof(int16_t));
  assert(sizeof(int32) == sizeof(int32_t));
  assert(sizeof(int64) == sizeof(int64_t));

  assert(uint16_read(buf)==0x100);
  assert(uint16_read(buf+1)==0x201);
  assert(uint16_read(buf+2)==0x302);
  assert(uint16_read(buf+3)==0x403);

  assert(uint16_read_big(buf)==0x0001);
  assert(uint16_read_big(buf+1)==0x0102);
  assert(uint16_read_big(buf+2)==0x0203);
  assert(uint16_read_big(buf+3)==0x0304);

  assert(uint32_read(buf)==0x03020100);
  assert(uint32_read(buf+1)==0x04030201);
  assert(uint32_read(buf+2)==0x05040302);
  assert(uint32_read(buf+3)==0x06050403);

  assert(uint32_read_big(buf)==0x00010203);
  assert(uint32_read_big(buf+1)==0x01020304);
  assert(uint32_read_big(buf+2)==0x02030405);
  assert(uint32_read_big(buf+3)==0x03040506);

  assert(uint64_read(buf)==0x0706050403020100ull);
  assert(uint64_read(buf+1)==0x0807060504030201ull);
  assert(uint64_read(buf+2)==0x0908070605040302ull);
  assert(uint64_read(buf+3)==0x0a09080706050403ull);

  assert(uint64_read_big(buf)==0x0001020304050607ull);
  assert(uint64_read_big(buf+1)==0x0102030405060708ull);
  assert(uint64_read_big(buf+2)==0x0203040506070809ull);
  assert(uint64_read_big(buf+3)==0x030405060708090aull);

  {
    uint16 us;
    uint32 ui;
    uint64 ul;
    size_t i;
    for (i=0; i<3; ++i) {
      uint16_unpack(buf+i,&us); assert(us == uint16_read(buf+i));
      uint32_unpack(buf+i,&ui); assert(ui == uint32_read(buf+i));
      uint64_unpack(buf+i,&ul); assert(ul == uint64_read(buf+i));
      uint16_unpack_big(buf+i,&us); assert(us == uint16_read_big(buf+i));
      uint32_unpack_big(buf+i,&ui); assert(ui == uint32_read_big(buf+i));
      uint64_unpack_big(buf+i,&ul); assert(ul == uint64_read_big(buf+i));

      zap(); uint16_pack(buf+i,0xc0de); assert(buf[i+2]=='_' && uint16_read(buf+i)==0xc0de);
      zap(); uint16_pack_big(buf+i,0xc0de); assert(buf[i+2]=='_' && uint16_read_big(buf+i)==0xc0de);
      zap(); uint32_pack(buf+i,0xfefec0de); assert(buf[i+4]=='_' && uint32_read(buf+i)==0xfefec0de);
      zap(); uint32_pack_big(buf+i,0xfefec0de); assert(buf[i+4]=='_' && uint32_read_big(buf+i)==0xfefec0de);
      zap(); uint64_pack(buf+i,0xfefec0dedeadbeef); assert(buf[i+8]=='_' && uint64_read(buf+i)==0xfefec0dedeadbeef);
      zap(); uint64_pack_big(buf+i,0xfefec0dedeadbeef); assert(buf[i+8]=='_' && uint64_read_big(buf+i)==0xfefec0dedeadbeef);
    }
  }

  {
    const char* mmapcopy;
    FILE* f;
    size_t mlen;
    assert(f=fopen("test/marshal.c","rb"));
    assert(fseek(f,0,SEEK_END)==0);
    flen=ftell(f);
    assert(flen>4096);
    fseek(f,0,SEEK_SET);
    assert(stdiocopy=malloc(flen));
    assert(fread(stdiocopy,1,flen,f)==flen);
    fclose(f);

    assert(openreadclose("test/marshal.c",&sa,4096)==1);
    assert(sa.len == flen);
    assert(byte_equal(sa.s,flen,stdiocopy));

    assert((mmapcopy=mmap_read("test/marshal.c",&mlen)) && mlen==flen);
    assert(byte_equal(sa.s,flen,mmapcopy));
    mmap_unmap(mmapcopy,mlen);
  }

  stralloc_free(&sa);

  assert(stralloc_ready(&sa,0x1000));
  assert(sa.a >= 0x1000);
  assert(stralloc_copyb(&sa,stdiocopy,0x900));
  assert(sa.len == 0x900);
  assert(stralloc_catb(&sa,stdiocopy+0x900,0x700));
  assert(sa.len == 0x1000);
  assert(byte_equal(sa.s,0x1000,stdiocopy));
  assert(stralloc_readyplus(&sa,0x1000));
  assert(sa.a >= 0x2000);
  assert(stralloc_readyplus(&sa,(size_t)-1)==0);
  assert(stralloc_ready(&sa,(size_t)-1)==0);

  stralloc_zero(&sa);
  assert(stralloc_copys(&sa,"hello, "));
  assert(stralloc_cats(&sa,"world!"));
  assert(stralloc_diffs(&sa,"hello, world!") == 0);
  assert(stralloc_equals(&sa,"hello, world!"));
  assert(stralloc_0(&sa));
  assert(str_equal(sa.s,"hello, world!"));
  assert(stralloc_equal(&sa,&sa));

  assert(stralloc_copym(&sa,"hallo",", ","welt","!\n"));
  assert(stralloc_equals(&sa,"hallo, welt!\n"));

  stralloc_zero(&sa);
  assert(stralloc_catlong(&sa,1));
  assert(stralloc_catint(&sa,2));
  assert(stralloc_catulong(&sa,3));
  assert(stralloc_catuint(&sa,4));
  assert(stralloc_equals(&sa,"1234"));
  assert(stralloc_catulong0(&sa,5678,6));
  assert(stralloc_equals(&sa,"1234005678"));
  stralloc_zero(&sa);
  assert(stralloc_catlong0(&sa,-5678,6));
  assert(stralloc_equals(&sa,"-005678"));
  assert(stralloc_chop(&sa));
  assert(stralloc_equals(&sa,"-00567"));
  assert(stralloc_chomp(&sa)==0);
  assert(stralloc_equals(&sa,"-00567"));
  assert(stralloc_cats(&sa,"\r\n"));
  assert(stralloc_equals(&sa,"-00567\r\n"));
  assert(stralloc_chomp(&sa)==2);
  assert(stralloc_equals(&sa,"-00567"));

  stralloc_zero(&sa);
  assert(stralloc_copys(&sa,"foo\nbar\r\nbaz"));
  {
    buffer b;
    stralloc s;
    size_t i;
    buffer_fromsa(&b,&sa);
    stralloc_init(&s);
    assert(buffer_getline_sa(&b,&s)==1 && stralloc_equals(&s,"foo\n"));
    assert(buffer_getnewline_sa(&b,&s)==1 && stralloc_equals(&s,"bar\r\n"));
    assert(buffer_getnewline_sa(&b,&s)==0 && stralloc_equals(&s,"baz"));
    buffer_close(&b);

    stralloc_free(&s);
    buffer_tosa(&b,&s);
    for (i=0; i<100; ++i)
      buffer_puts(&b,"foo bar baz!\n");
    buffer_flush(&b);
    assert(s.len==100*sizeof("foo bar baz!"));
    for (i=0; i<100; ++i)
      assert(byte_equal(s.s+i*sizeof("foo bar baz!"),sizeof("foo bar baz!"),"foo bar baz!\n"));
  }

  return 0;
}

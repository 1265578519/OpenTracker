#include <assert.h>
#include <uint16.h>
#include <uint32.h>
#include <uint64.h>
#include <byte.h>

int main() {
  char buf[8];

  {
    uint16 a;

    buf[0]=buf[1]=0;
    uint16_pack_big(buf,0x1234);
    assert(buf[0]==0x12 && buf[1]==0x34);
    uint16_unpack_big(buf,&a);
    assert(a==0x1234);
    assert(uint16_read_big(buf)==0x1234);

    buf[0]=buf[1]=0;
    uint16_pack(buf,0x1234);
    assert(buf[0]==0x34 && buf[1]==0x12);
    uint16_unpack(buf,&a);
    assert(a==0x1234);
    assert(uint16_read(buf)==0x1234);
  }

  {
    uint32 a;

    buf[0]=buf[1]=buf[2]=buf[3]=0;
    uint32_pack_big(buf,0x12345678);
    assert(buf[0]==0x12 && buf[1]==0x34 && buf[2]==0x56 && buf[3]==0x78);
    uint32_unpack_big(buf,&a);
    assert(a==0x12345678);
    assert(uint32_read_big(buf)==0x12345678);

    buf[0]=buf[1]=buf[2]=buf[3]=0;
    uint32_pack(buf,0x12345678);
    assert(buf[0]==0x78 && buf[1]==0x56 && buf[2]==0x34 && buf[3]==0x12);
    uint32_unpack(buf,&a);
    assert(a==0x12345678);
    assert(uint32_read(buf)==0x12345678);
  }

  {
    uint64 a;
    unsigned int i;

    byte_zero(buf,sizeof(buf));
    uint64_pack_big(buf,0x0102030405060708ull);
    for (i=0; i<8; ++i) assert(buf[i]==i+1);
    uint64_unpack_big(buf,&a);
    assert(a==0x0102030405060708ull);
    assert(uint64_read_big(buf)==0x0102030405060708ull);

    byte_zero(buf,sizeof(buf));
    uint64_pack(buf,0x0102030405060708ull);
    for (i=0; i<8; ++i) assert(buf[7-i]==i+1);
    uint64_unpack(buf,&a);
    assert(a==0x0102030405060708ull);
    assert(uint64_read(buf)==0x0102030405060708ull);
  }


}

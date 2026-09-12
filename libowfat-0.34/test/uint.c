#include <assert.h>
#include <libowfat/uint16.h>
#include <libowfat/uint32.h>
#include <libowfat/uint64.h>
#include <libowfat/byte.h>

int main() {
  char buf[8];

  {
    uint16 a;

    buf[0]=buf[1]=0;
    uint16_pack_big(buf,0xabcd);
    assert((unsigned char)buf[0]==0xab && (unsigned char)buf[1]==0xcd);
    uint16_unpack_big(buf,&a);
    assert(a==0xabcd);
    assert(uint16_read_big(buf)==0xabcd);

    buf[0]=buf[1]=0;
    uint16_pack(buf,0xabcd);
    assert((unsigned char)buf[0]==0xcd && (unsigned char)buf[1]==0xab);
    uint16_unpack(buf,&a);
    assert(a==0xabcd);
    assert(uint16_read(buf)==0xabcd);
  }

  {
    uint32 a;

    buf[0]=buf[1]=buf[2]=buf[3]=0;
    uint32_pack_big(buf,0x89abcdef);
    assert((unsigned char)buf[0]==0x89 && (unsigned char)buf[1]==0xab &&
	   (unsigned char)buf[2]==0xcd && (unsigned char)buf[3]==0xef);
    uint32_unpack_big(buf,&a);
    assert(a==0x89abcdef);
    assert(uint32_read_big(buf)==0x89abcdef);

    buf[0]=buf[1]=buf[2]=buf[3]=0;
    uint32_pack(buf,0x89abcdef);
    assert((unsigned char)buf[3]==0x89 && (unsigned char)buf[2]==0xab &&
	   (unsigned char)buf[1]==0xcd && (unsigned char)buf[0]==0xef);
    uint32_unpack(buf,&a);
    assert(a==0x89abcdef);
    assert(uint32_read(buf)==0x89abcdef);
  }

  {
    uint64 a;
    unsigned int i;

    byte_zero(buf,sizeof(buf));
    uint64_pack_big(buf,0x8182838485868788ull);
    for (i=0; i<8; ++i) assert((unsigned char)buf[i]==i+0x81);
    uint64_unpack_big(buf,&a);
    assert(a==0x8182838485868788ull);
    assert(uint64_read_big(buf)==0x8182838485868788ull);

    byte_zero(buf,sizeof(buf));
    uint64_pack(buf,0x8182838485868788ull);
    for (i=0; i<8; ++i) assert((unsigned char)buf[7-i]==i+0x81);
    uint64_unpack(buf,&a);
    assert(a==0x8182838485868788ull);
    assert(uint64_read(buf)==0x8182838485868788ull);
  }


}

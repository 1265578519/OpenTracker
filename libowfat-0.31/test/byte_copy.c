#include "byte.h"
#include <assert.h>
#include "errmsg.h"
#include <string.h>

char buf[4096];
char text[128];

int main() {
  memset(buf,0,sizeof(buf));
  strcpy(text,"this is a test!\n");

//  carp("both aligned");
  byte_copy(buf,16,text);
  assert(memcmp(buf,"this is a test!\n\0",18)==0);

  memset(buf,0,sizeof(buf));
//  carp("destination aligned, source unaligned");
  byte_copy(buf,15,text+1);
  assert(memcmp(buf,"his is a test!\n\0\0",18)==0);

  memset(buf,0,sizeof(buf));
//  carp("destination unaligned, source aligned");
  byte_copy(buf+1,15,text);
  assert(memcmp(buf,"\0this is a test!\0\0",18)==0);

  memset(buf,0,sizeof(buf));
//  carp("both unaligned");
  byte_copy(buf+1,10,text+3);
  assert(memcmp(buf,"\0s is a tes\0\0",14)==0);

  memset(buf,0,sizeof(buf));
  byte_copyr(buf,16,text);
  assert(memcmp(buf,"this is a test!\n\0",18)==0);

  memset(buf,0,sizeof(buf));
  byte_copyr(buf,15,text+1);
  assert(memcmp(buf,"his is a test!\n\0\0",18)==0);

  memset(buf,0,sizeof(buf));
  byte_copyr(buf+1,15,text);
  assert(memcmp(buf,"\0this is a test!\0\0",18)==0);

  memset(buf,0,sizeof(buf));
  byte_copyr(buf+1,10,text+3);
  assert(memcmp(buf,"\0s is a tes\0\0",14)==0);

  memset(buf,0,sizeof(buf));
  byte_copy(buf,16,text);
  byte_copy(buf,16,buf+1);
  assert(memcmp(buf,"his is a test!\n\0\0",18)==0);

  memset(buf,0,sizeof(buf));
  byte_copy(buf,16,text);
  byte_copyr(buf+1,16,buf);
  assert(memcmp(buf,"tthis is a test!\n",18)==0);

  assert(byte_diff(text,15,"this is a test!")==0);
  assert(byte_diff(text,16,"this is a test!")>0);
  assert(byte_diff("this is a test!",16,text)<0);

  assert(byte_chr("0123456789abcdef",17,'9')==9);
  assert(byte_rchr("0123456789abcdef",17,'9')==9);
  assert(byte_chr("0123456789abcdef",17,'A')==17);
  assert(byte_rchr("0123456789abcdef",17,'A')==17);

  byte_zero(buf,16);
  assert(byte_equal(buf,16,"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"));

  return 0;
}

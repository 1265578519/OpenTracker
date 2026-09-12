#include <unistd.h>
#include "dns.h"
#include "taia.h"
#include "uint32.h"

static uint32 seed[32];
static uint32 in[12];
static uint32 out[8];
static int outleft = 0;

#define ROTATE(x,b) (((x) << (b)) | ((x) >> (32 - (b))))
#define MUSH(i,b) x = t[i] += (((x ^ seed[i]) + sum) ^ ROTATE(x,b));

static void surf(void)
{
  uint32 t[12]; uint32 x; uint32 sum = 0;
  int r; int i; int loop;

  for (i = 0;i < 12;++i) t[i] = in[i] ^ seed[12 + i];
  for (i = 0;i < 8;++i) out[i] = seed[24 + i];
  x = t[11];
  for (loop = 0;loop < 2;++loop) {
    for (r = 0;r < 16;++r) {
      sum += 0x9e3779b9;
      MUSH(0,5) MUSH(1,7) MUSH(2,9) MUSH(3,13)
      MUSH(4,5) MUSH(5,7) MUSH(6,9) MUSH(7,13)
      MUSH(8,5) MUSH(9,7) MUSH(10,9) MUSH(11,13)
    }
    for (i = 0;i < 8;++i) out[i] ^= t[i + 4];
  }
}

void dns_random_init(const char data[128])
{
  int i;
  struct taia t;
  char tpack[16];

  for (i = 0;i < 32;++i)
    uint32_unpack(data + 4 * i,seed + i);

  taia_now(&t);
  taia_pack(tpack,&t);
  for (i = 0;i < 4;++i)
    uint32_unpack(tpack + 4 * i,in + 4 + i);

  in[8] = getpid();
#ifndef __MINGW32__
  in[9] = getppid();
#endif
  /* more space in 10 and 11, but this is probably enough */
}

unsigned int dns_random(unsigned int n)
{
  if (!n) return 0;

  if (!outleft) {
    if (!++in[0]) if (!++in[1]) if (!++in[2]) ++in[3];
    surf();
    outleft = 8;
  }

  return out[--outleft] % n;
}

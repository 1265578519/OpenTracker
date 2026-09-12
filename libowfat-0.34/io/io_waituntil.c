#include "io_internal.h"
#include "safemult.h"

void io_waituntil(tai6464 t) {
  uint64 x,y;
  tai6464 now,diff;
  taia_now(&now);
  if (taia_less(&t,&now))
    y=0;
  else {
    taia_sub(&diff,&t,&now);
    if (!umult64(diff.sec.x,1000,&x) || (y=x+diff.nano/1000000)<x)
      y=-1;	/* overflow; wait indefinitely */
    if (!y && diff.nano) y=1;
  }
  io_waituntil2(y);
}

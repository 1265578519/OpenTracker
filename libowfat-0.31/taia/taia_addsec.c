#include "taia.h"

/* XXX: breaks tai encapsulation */

void taia_addsec(struct taia *t,const struct taia *u,long secs)
{
  t->sec.x = u->sec.x + (uint64)(int64)secs;
  t->nano = u->nano;
  t->atto = u->atto;
}

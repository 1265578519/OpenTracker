#include "taia.h"

/* XXX: breaks tai encapsulation */

void taia_half(tai6464* t,const tai6464* u) {
  t->atto = u->atto >> 1;
  if (u->nano & 1) t->atto += (uint32)500000000UL;
  t->nano = u->nano >> 1;
  if (u->sec.x & 1) t->nano += (uint32)500000000UL;
  t->sec.x = u->sec.x >> 1;
}

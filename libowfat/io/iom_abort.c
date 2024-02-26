#include "io_internal.h"

int iom_abort(iomux_t* c) {
  c->working=-2;
#ifdef __dietlibc__
  return cnd_broadcast(&c->sem);
#else
  return sem_post(&c->sem);
#endif
}

#include "io_internal.h"

int iom_abort(iomux_t* c) {
  c->working=-2;
#ifdef __dietlibc__
  return cnd_broadcast(&c->sem) == thrd_success ? 0 : -1;
#elif defined(__APPLE__)
  return pthread_cond_broadcast(&c->sem) == 0 ? 0 : -1;
#else
  return sem_post(&c->sem);
#endif
}

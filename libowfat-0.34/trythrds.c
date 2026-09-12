#include <sys/time.h>
#include <threads.h>

int worker(void* arg) {
  (void)arg;
  return 0;
}

int main() {
  thrd_t x;
  if (thrd_create(&x,worker,0)==-1)
    return 111;
  if (thrd_join(x,NULL)==-1)
    return 111;
  cnd_t c;
  mtx_t m;
  mtx_init(&m, mtx_plain);
  mtx_lock(&m);
  mtx_unlock(&m);
  cnd_init(&c);
  cnd_timedwait(&c, &m, (struct timespec[]){ [0].tv_sec=1 });
  cnd_destroy(&c);
  mtx_destroy(&m);
  return 0;
}

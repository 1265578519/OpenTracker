#include "io.h"
#include <stdio.h>
#include <threads.h>
#include <unistd.h>

iomux_t c;

int worker(void* arg) {
  uintptr_t i=(uintptr_t)arg;
  char buf[100];
  int64 s;
  int events;

  write(1,buf,sprintf(buf,"starting thread %ld\n",i));

  switch (iom_wait(&c,&s,&events,1000)) {
  case -1:
    perror("iom_wait");
    return -1;
  case 0:
    write(1,buf,sprintf(buf,"timeout in thread %ld\n",i));
    return 0;
  case 1:
    write(1,buf,sprintf(buf,"input found by thread %8ld\n",i));
    return 1;
  }

  return 0;
}

int main() {
  iom_init(&c);
  if (iom_add(&c,0,IOM_READ)==-1) {
    perror("iom_add");
    return 1;
  }
  thrd_t x[4];
  int i;
  puts("launching threads");
  for (i=0; i<4; ++i)
    if (thrd_create(&x[i],worker,(void*)(uintptr_t)i)==-1) {
      perror("thrd_create");
      return 111;
    }

//  sleep(1);
//  iom_abort(&c);

  puts("joining threads");
  int r;
  for (i=0; i<4; ++i) {
    if (thrd_join(x[i],&r)==-1) {
      perror("thrd_join");
      return 111;
    }
    printf("thread %d returned %d\n",i,r);
  }

  return 0;
}

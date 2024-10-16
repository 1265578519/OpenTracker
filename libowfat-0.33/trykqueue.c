#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

int main() {
  int kq=kqueue();
  struct kevent kev;
  struct timespec ts;
  if (kq==-1) return 111;
  EV_SET(&kev, 0 /* fd */, EVFILT_READ, EV_ADD|EV_ENABLE, 0, 0, 0);
  ts.tv_sec=0; ts.tv_nsec=0;
  if (kevent(kq,&kev,1,0,0,&ts)==-1) return 111;

  {
    struct kevent events[100];
    int i,n;
    ts.tv_sec=1; ts.tv_nsec=0;
    switch (n=kevent(kq,0,0,events,100,&ts)) {
    case -1: return 111;
    case 0: puts("no data on fd #0"); break;
    }
    for (i=0; i<n; ++i) {
      printf("ident %d, filter %d, flags %d, fflags %d, data %d\n",
             events[i].ident,events[i].filter,events[i].flags,
             events[i].fflags,events[i].data);
    }
  }
  return 0;
}

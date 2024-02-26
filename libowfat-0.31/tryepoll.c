#include <inttypes.h>
#include <sys/epoll.h>
#include <stdio.h>

int main() {
  int efd=epoll_create(10);
  struct epoll_event x;
  if (efd==-1) return 111;
  x.events=EPOLLIN;
  x.data.fd=0;
  if (epoll_ctl(efd,EPOLL_CTL_ADD,0 /* fd */,&x)==-1) return 111;
  {
    int n;
    struct epoll_event y[100];
    if ((n=epoll_wait(efd,y,100,1000))==-1) return 111;
    if (n>0)
      printf("event %d on fd #%d\n",y[0].events,y[0].data.fd);
  }
  return 0;
}

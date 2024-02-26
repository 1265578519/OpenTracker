#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <sys/devpoll.h>

main() {
  int fd=open("/dev/poll",O_RDWR);
  struct pollfd p[100];
  int i,r;
  dvpoll_t timeout;
  p[0].fd=0;
  p[0].events=POLLIN;
  write(fd,p,sizeof(struct pollfd));
  timeout.dp_timeout=100;	/* milliseconds? */
  timeout.dp_nfds=1;
  timeout.dp_fds=p;
  r=ioctl(fd,DP_POLL,&timeout);
  for (i=0; i<r; ++i)
    printf("event %d on fd #%d\n",p[i].revents,p[i].fd);
}

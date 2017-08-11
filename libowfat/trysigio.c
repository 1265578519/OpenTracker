#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/poll.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>

int main() {
  int signum=SIGRTMIN+1;
  sigset_t ss;
  sigemptyset(&ss);
  sigaddset(&ss,signum);
  sigaddset(&ss,SIGIO);
  sigprocmask(SIG_BLOCK,&ss,0);

  fcntl(0 /* fd */,F_SETOWN,getpid());
  fcntl(0 /* fd */,F_SETSIG,signum);
#if defined(O_ONESIGFD) && defined(F_SETAUXFL)
  fcntl(0 /* fd */, F_SETAUXFL, O_ONESIGFD);
#endif
  fcntl(0 /* fd */,F_SETFL,fcntl(0 /* fd */,F_GETFL)|O_NONBLOCK|O_ASYNC);

  {
    siginfo_t info;
    struct timespec timeout;
    int r;
    timeout.tv_sec=1; timeout.tv_nsec=0;
    switch ((r=sigtimedwait(&ss,&info,&timeout))) {
    case SIGIO:
      /* signal queue overflow */
      signal(signum,SIG_DFL);
      /* do poll */
      break;
    default:
      if (r==signum) {
	printf("event %c%c on fd #%d\n",info.si_band&POLLIN?'r':'-',info.si_band&POLLOUT?'w':'-',info.si_fd);
      }
    }
  }
  return 0;
}

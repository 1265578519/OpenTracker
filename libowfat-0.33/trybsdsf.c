/* for macos X, don't ask */
#define SENDFILE 1

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

int main() {
  struct sf_hdtr hdr;
  struct iovec v[17+23];
  int r,fd=1;
  off_t sbytes;
  hdr.headers=v; hdr.hdr_cnt=17;
  hdr.trailers=v+17; hdr.trl_cnt=23;
  r=sendfile(0,1,37,42,&hdr,&sbytes,0);
  (void)r;
  return 0;
}

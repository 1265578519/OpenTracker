#include "io.h"

char buf[65536];
int64 readpos = 0;
int64 writepos = 0;
int flageof = 0;

int main()
{
  int64 r;

  if (!io_fd(0)) return 111;
  if (!io_fd(1)) return 111;

  for (;;) {
    if (flageof && writepos >= readpos) return 0;

    if (!flageof && readpos < sizeof buf) io_wantread(0);
    if (writepos < readpos) io_wantwrite(1);
    io_wait();
    if (!flageof && readpos < sizeof buf) io_dontwantread(0);
    if (writepos < readpos) io_dontwantwrite(1);

    if (!flageof && readpos < sizeof buf) {
      r = io_tryread(0,buf + readpos,sizeof buf - readpos);
      if (r <= -2) return 111; /* read error other than EAGAIN */
      if (r == 0) flageof = 1;
      if (r > 0) readpos += r;
    }

    if (writepos < readpos) {
      r = io_trywrite(1,buf + writepos,readpos - writepos);
      if (r <= -2) return 111; /* write error other than EAGAIN */
      if (r > 0) {
	writepos += r;
	if (writepos == readpos) readpos = writepos = 0;
	/* if writepos is big, might want to left-shift buffer here */
      }
    }
  }
}

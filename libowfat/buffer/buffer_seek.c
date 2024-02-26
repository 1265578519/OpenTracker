#include <unistd.h>

#include "buffer.h"

ssize_t buffer_seek(buffer* b,size_t len) {
  size_t leftinbuf = b->n - b->p;
  ssize_t r;

  if ((ssize_t)len < 0) return -1;	// can't signal back how much we read, so error out
  r = (ssize_t)len;

  if (len <= leftinbuf) {
    b->p += len;
    return len;
  } else {
    // want to skip more than there was in the buffer
    len -= leftinbuf;
    b->p = 0;	// clear buffer
    b->n = 0;

    /* change position in underlying file */
    if (b->fd != -1 && len > b->a &&
        lseek(b->fd, len, SEEK_CUR) != -1) return len;
    // either we have no fd or input is not seekable
    // call read repeatedly
    while (len > 0) {
      ssize_t r = buffer_feed(b);
      if (r < 0) return -1;
      if (r == 0) break;
      if ((size_t)r > len) {	// awesome, we are done
	b->p = len;
	return r;
      }
      len -= r;
      b->p = b->n = 0;
    }
    return r;
  }
}

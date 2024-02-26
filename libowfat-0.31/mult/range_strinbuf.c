#include <rangecheck.h>

/* does an ASCIIZ string starting at "ptr" lie in buf[0..len-1]? */
int range_strinbuf(const void* buf,size_t len,const void* stringstart) {
  const char* x;
  const char* y;
  if (!range_ptrinbuf(buf,len,x=stringstart)) return 0;
  y=x+len;
  for (; x<y && *x; ++x);
  return (x<y);
}


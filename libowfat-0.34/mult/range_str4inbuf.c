#include <rangecheck.h>
#include <inttypes.h>

/* does an UTF-32 string starting at "ptr" lie in buf[0..len-1]? */
int range_str4inbuf(const void* buf,size_t len,const void* stringstart) {
  const uint32_t* x;
  const uint32_t* y;
  if (!range_ptrinbuf(buf,len,x=stringstart)) return 0;
  y=(const uint32_t*)((char*)x+len);
  for (; x+1<=y && *x; ++x);
  return (x+1<=y);
}


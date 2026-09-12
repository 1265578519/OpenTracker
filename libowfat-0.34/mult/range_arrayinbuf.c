#include <rangecheck.h>
#include <safemult.h>

/* does an array of "elements" members of size "membersize" starting at
 * "arraystart" lie inside buf1[0..len-1]? */
int range_arrayinbuf(const void* buf1,size_t len,
		     const void* arraystart,size_t elements,size_t membersize) {
  size_t alen;
  if (sizeof(alen)==8) {
    uint64 x;
    if (!umult64(elements,membersize,&x)) return 0;
    alen=x;
  } else {
    unsigned long long t=(unsigned long long)elements*membersize;
    alen=t;			/* this strips the upper 32 bits of t */
    if (alen!=t) return 0;	/* if that changes something, we overflowed */
  }
  return range_bufinbuf(buf1,len,arraystart,alen);
}


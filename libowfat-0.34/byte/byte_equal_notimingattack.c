#include "byte.h"

/* If you need to compare a password or a hash value, the timing of the
 * comparison function can give valuable clues to the attacker. Let's
 * say the password is 123456 and the attacker tries abcdef. If the
 * comparision function fails at the first byte without looking at the
 * other bytes, then the attacker can measure the difference in runtime
 * and deduce which byte was wrong, reducing the attack space from
 * exponential to polynomial. */
int byte_equal_notimingattack(const void* a, size_t len,const void* b) {
  size_t i;
  const unsigned char* x=(const unsigned char*)a;
  const unsigned char* y=(const unsigned char*)b;
  unsigned char res=0;
  for (i=0; i<len; ++i) {
    res |= (x[i]^y[i]);
  }
  return res==0;
}

#include <byte.h>
#include <string.h>

int byte_start(const void* a,size_t alen,const void* b,size_t blen) {
  return blen<=alen && !memcmp(a,b,blen);
}

#ifdef UNITTEST
#include <assert.h>
int main() {
  static char buf[]="The quick brown fox jumps over the lazy dog";
  assert(byte_start(buf,sizeof(buf)-1,"The ",4));
  assert(!byte_start(buf,sizeof(buf)-1,"the ",4));
  assert(!byte_start(buf,3,buf,9));
}
#endif

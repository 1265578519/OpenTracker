#include <byte.h>
#undef byte_starts
#include <string.h>

int byte_starts(const void* a,size_t alen,const char* s) {
  size_t i;
  for (i=0; i<alen; ++i) {
    if (s[i]==0) return 1;
    if (((const char*)a)[i] != s[i]) return 0;
  }
  return s[i]==0;
}

#ifdef UNITTEST
#include <assert.h>

int main() {
  static char buf[]="The quick brown fox jumps over the lazy dog";
  assert(byte_starts(buf,sizeof(buf)-1,"The "));
  assert(!byte_starts(buf,sizeof(buf)-1,"the "));
  assert(!byte_starts(buf,2,"The "));
  assert(byte_starts("The ",4,"The "));
}
#endif

#include "scan.h"

size_t scan_varint(const char* in,size_t len, unsigned long long* n) {
  size_t i;
  unsigned long long l;
  if (len==0) return 0;
  for (l=0, i=0; i<len; ++i) {
    if (i*7+7>sizeof(l)*8)	// value too large to fit in destination integer!
      return 0;
    l+=(unsigned long long)(in[i]&0x7f) << (i*7);
    if (!(in[i]&0x80)) {
      if (in[i]==0) return 0;	// libowfat extension: reject non-minimal encoding
      *n=l;
      return i+1;
    }
  }
  return 0;
}

#ifdef __ELF__
size_t scan_pb_type0_int(const char* dest,size_t len,unsigned long long* l) __attribute__((alias("scan_varint")));
#else
size_t scan_pb_type0_int(const char* dest,size_t len,unsigned long long* l) {
  return scan_varint(dest,len,l);
}
#endif

#ifdef UNITTEST
#include <assert.h>

int main() {
  unsigned long long n;
  assert(scan_varint("\x05",1,&n)==1 && n==5);
  assert(scan_varint("\x80\x00",2,&n)==0);	// non-minimal encoding
  assert(scan_varint("\x80\x80",2,&n)==0);	// truncated (last byte in sequence has high bit 0)
  assert(scan_varint("\x80\x80\x80\x80\x80\x80\x80\x80\x80\x01",10,&n)==0);	// value too large
  return 0;
}
#endif

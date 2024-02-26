#include "fmt.h"

size_t fmt_pb_int(char* dest,size_t fieldno,unsigned long long l) {
  size_t n=fmt_pb_tag(dest,fieldno,0);
  return n+fmt_pb_type0_int(dest?dest+n:0,l);
}


#include "fmt.h"

size_t fmt_pb_sint(char* dest,size_t fieldno,signed long long l) {
  size_t n=fmt_pb_tag(dest,fieldno,0);
  return n+fmt_pb_type0_sint(dest?dest+n:0,l);
}

#include "fmt.h"
#include "rangecheck.h"

size_t fmt_pb_string(char* dest,size_t fieldno,const char* s,size_t l) {
  size_t n=fmt_pb_tag(dest,fieldno,2);
  size_t m;
  if (add_of(m,fmt_pb_type2_string(NULL,s,l),n)) return 0;
  return n+fmt_pb_type2_string(dest?dest+n:0,s,l);
}

#include "fmt.h"

size_t fmt_pb_double(char* dest,size_t fieldno,double d) {
  size_t n=fmt_pb_tag(dest,fieldno,1);
  return n+fmt_pb_type1_double(dest?dest+n:0,d);
}


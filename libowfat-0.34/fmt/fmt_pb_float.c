#include "fmt.h"

size_t fmt_pb_float(char* dest,size_t fieldno,float f) {
  size_t n=fmt_pb_tag(dest,fieldno,5);
  return n+fmt_pb_type5_float(dest?dest+n:0,f);
}

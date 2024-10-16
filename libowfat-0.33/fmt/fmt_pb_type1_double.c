#include "fmt.h"
#include "compiletimeassert.h"

size_t fmt_pb_type1_double(char* dest,double d) {
  union {
    double d;
    uint64_t u;
  } u;
  compiletimeassert(sizeof(double) == 8);
  u.d=d;
  return fmt_pb_type1_fixed64(dest,u.u);
}

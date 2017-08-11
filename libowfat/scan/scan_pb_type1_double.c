#include "scan.h"
#include "compiletimeassert.h"

size_t scan_pb_type1_double(const char* in,size_t len,double* d) {
  union {
    double d;
    uint64_t u;
  } u;
  compiletimeassert(sizeof(double)==8);
  size_t n=scan_pb_type1_fixed64(in,len,&u.u);
  if (n) *d=u.d;
  return n;
}

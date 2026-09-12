#include "scan.h"
#include "compiletimeassert.h"

size_t scan_pb_type5_float(const char* in,size_t len,float* f) {
  union {
    float f;
    uint32_t u;
  } u;
  compiletimeassert(sizeof(float)==4);
  size_t n=scan_pb_type5_fixed32(in,len,&u.u);
  if (n) *f=u.f;
  return n;
}

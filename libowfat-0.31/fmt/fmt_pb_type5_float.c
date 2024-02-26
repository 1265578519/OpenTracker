#include "fmt.h"
#include "compiletimeassert.h"

size_t fmt_pb_type5_float(char* dest,float f) {
  union {
    float f;
    uint32_t u;
  } u;
  compiletimeassert(sizeof(float) == 4);
  u.f=f;
  return fmt_pb_type5_fixed32(dest,u.u);
}

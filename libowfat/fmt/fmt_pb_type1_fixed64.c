#include "fmt.h"
#include "uint64.h"

size_t fmt_pb_type1_fixed64(char* dest,uint64_t l) {
  if (dest) uint64_pack(dest,l);
  return 8;
}

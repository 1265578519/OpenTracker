#include "fmt.h"

size_t fmt_pb_type0_sint(char* dest,signed long long l) {
  return fmt_varint(dest,(l << 1) ^ (l >> (sizeof(l)*8-1)));
}

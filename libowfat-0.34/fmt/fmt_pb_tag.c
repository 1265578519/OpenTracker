#include "fmt.h"

size_t fmt_pb_tag(char* dest,size_t fieldno,unsigned char type) {
  if (type>5 || (fieldno >> (sizeof(fieldno)*8-3))) return 0;
  return fmt_varint(dest,(type&7) | (fieldno<<3));
}

#ifndef __GNUC__

#include "fmt.h"

size_t fmt_escapecharhtml(char* dest,uint32_t ch) {
  return fmt_escapecharxml(dest,ch);
}

#endif

#include "fmt.h"

char fmt_tohex(char c) {
  return (char)(c>=10?c-10+'a':c+'0');
}


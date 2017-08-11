#include "array.h"
#include "str.h"

void array_cats(array* to,const char* from) {
  array_catb(to,from,str_len(from));
}

#include "buffer.h"

int main() {
  buffer input;
  char x;
  buffer_mmapread(&input,"/etc/passwd");
  while (buffer_get(&input,&x,1)==1) {
    buffer_put(buffer_1,&x,1);
    if (x=='\n') break;
  }
  buffer_flush(buffer_1);
  buffer_close(&input);
  return 0;
}

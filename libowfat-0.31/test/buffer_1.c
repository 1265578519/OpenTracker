#include "buffer.h"

int main() {
  buffer_puts(buffer_1,"ulong: ");
  buffer_putulong(buffer_1,23);
  buffer_puts(buffer_1,"\nlong: ");
  buffer_putlong(buffer_1,-23);
  buffer_puts(buffer_1,"\n8long: ");
  buffer_put8long(buffer_1,23);
  buffer_puts(buffer_1,"\nxlong:");
  buffer_putspace(buffer_1);
  buffer_putxlong(buffer_1,23);
  buffer_putsflush(buffer_1,"\n");
  buffer_putsalign(buffer_1,"aligned\n");
  buffer_flush(buffer_1);
  return 0;
}

#include <stdlib.h>
#ifdef A
#include <alloca.h>
#else
#include <malloc.h>
#endif

int main() {
  char* c=alloca(23);
  (void)c;
  return 0;
}

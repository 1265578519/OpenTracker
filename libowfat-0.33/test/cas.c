#include "CAS.h"
#include <assert.h>

int main() {
  size_t x;
  x=0;
  assert((compare_and_swap(&x,1,2)==0 , x==0));
  assert((compare_and_swap(&x,0,2)==1 , x==2));

  assert((atomic_add(&x,5) , x == 7));
  assert((atomic_add(&x,-5) , x == 2));

  assert(atomic_add_return(&x,5) == 7);
  assert(atomic_add_return(&x,-5) == 2);

  atomic_inc(&x); assert(x==3);
  assert(atomic_inc_return(&x) == 4);
  atomic_dec(&x); assert(x==3);
  assert(atomic_dec_return(&x) == 2);

  assert((atomic_or(&x,1) , x==3));
  assert((atomic_and(&x,~1) , x==2));
}

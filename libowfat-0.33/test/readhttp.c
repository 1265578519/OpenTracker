#include "stralloc.h"
#include "buffer.h"
#include "byte.h"

/* this demonstrates the buffer_get_token_sa_pred interface;
 * ishttp is the predicate that determines when buffer_get_token_sa_pred
 * stops reading.  It is supposed to return 0 if the token is not
 * complete and 1 if it is.  It can also return -1 to indicate an error,
 * for example you could enforce a timeout here or limit the token size. */

int ishttp(stralloc* sa) {
  return (sa->len>4 && byte_equal(sa->s+sa->len-4,4,"\r\n\r\n"));
}

int main() {
  stralloc sa;
  int res;

  stralloc_init(&sa);
  res=buffer_get_token_sa_pred(buffer_0,&sa,ishttp);
  buffer_puts(buffer_1,"buffer_get_token_sa_pred returned ");
  buffer_putlong(buffer_1,res);
  buffer_putsflush(buffer_1,".\n\n");
  buffer_putsa(buffer_1,&sa);
  buffer_flush(buffer_1);
  return 0;
}

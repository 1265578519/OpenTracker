#include <assert.h>
#include <string.h>
#include <textcode.h>

int main() {
  char buf[100];
  size_t l;
  assert(scan_jsonescape("fnord",buf,&l)==5 && l==5 && !memcmp(buf,"fnord",5));
  /* is \n properly unescaped? */
  assert(scan_jsonescape("a\\nb",buf,&l)==4 && l==3 && !memcmp(buf,"a\nb",3));
  assert(scan_jsonescape("a\\rb",buf,&l)==4 && l==3 && !memcmp(buf,"a\rb",3));
  assert(scan_jsonescape("a\\bb",buf,&l)==4 && l==3 && !memcmp(buf,"a\bb",3));
  assert(scan_jsonescape("a\\fb",buf,&l)==4 && l==3 && !memcmp(buf,"a\fb",3));
  assert(scan_jsonescape("a\\tb",buf,&l)==4 && l==3 && !memcmp(buf,"a\tb",3));
  assert(scan_jsonescape("a\\\\b",buf,&l)==4 && l==3 && !memcmp(buf,"a\\b",3));
  assert(scan_jsonescape("a\\/b",buf,&l)==4 && l==3 && !memcmp(buf,"a/b",3));
  assert(scan_jsonescape("a\\\"b",buf,&l)==4 && l==3 && !memcmp(buf,"a\"b",3));
  /* does a double quote end the string? */
  assert(scan_jsonescape("a\"b",buf,&l)==1 && l==1 && !memcmp(buf,"a",1));
  /* how about unicode escape */
  assert(scan_jsonescape("a\\u005cb",buf,&l)==8 && l==3 && !memcmp(buf,"a\\b",3));
  /* a trailing surrogate pair with no lead before it */
  assert(scan_jsonescape("a\\udead\"",buf,&l)==1);
  /* a lead surrogate pair with no trailer behind it */
  assert(scan_jsonescape("a\\udafd\"",buf,&l)==1);
  assert(scan_jsonescape("a\\udafd\\udafd",buf,&l)==1);
  assert(scan_jsonescape("a\\udafd0",buf,&l)==1);
  /* correct surrogate pair */
  assert(scan_jsonescape("a\\ud834\\udd1eb",buf,&l)==14 && l==6 && !memcmp(buf,"a\xf0\x9d\x84\x9e""b",6));
  /* how about some incorrect UTF-8? */
  assert(scan_jsonescape("a\xc0\xaf",buf,&l)==1 && l==1 && !memcmp(buf,"a",1));
}

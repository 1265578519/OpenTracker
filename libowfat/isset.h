/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef ISSET_H
#define ISSET_H

/*
 How to check whether a preprocessor symbol is defined or not?
 Difficulty: it should work both in the preprocessor:

#if is_set(FOO)

 and in C code (and then no code should be generated)

  if (is_set(FOO))

 We will allow both
 
#define FOO

 and

#define FOO 1
*/

/* Original question posed by Linus Torvalds
 * https://plus.google.com/102150693225130002912/posts/9gntjh57dXt
 * Solution by comex
 * https://gist.github.com/2365372
 */

#define is_set(macro) is_set_(macro)
#define macrotest_1 ,
#define macrotest_ ,
#define is_set_(value) is_set__(macrotest_##value)
#define is_set__(comma) is_set___(comma 1, 0)
#define is_set___(_, v, ...) v

#endif

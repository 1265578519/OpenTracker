/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef RANGECHECK_H
#define RANGECHECK_H

#include <inttypes.h>
#include <stddef.h>

#include <libowfat/compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

/* We are trying to achieve that gcc has to inline the function and we
 * don't want it to emit a copy of the function.  This can be done with
 * static inline or with extern inline.  static inline tells gcc to not
 * emit a copy unless someone is using & to take a pointer, which nobody
 * is ever supposed to do.  extern inline tells gcc to not ever emit a
 * copy.
 *
 * Unfortunately, the C99 standard defines extern inline to mean "always
 * emit a copy for external reference", so this causes duplicate symbol
 * linking errors.  gcc signals C99 inline expansion mode by defining
 * __GNUC_STDC_INLINE__ and it then has an attribute gnu_inline to
 * switch back to GNU behavior.  So that's what we are doing.  Taking
 * the address of one of these functions is considered a user error.
 *
 * We are so anal about inlining here because these checks can in most
 * cases be optimized away.  In particular, if you call this function
 * often, gcc can see that some of the basic checks are done repeatedly
 * and not do them again.  But this only works if the function is
 * inlined. */

#if defined(__GNUC_STDC_INLINE__)
#define __gnuinline __attribute__((gnu_inline))
#else
#define __gnuinline
#endif

#if defined(__GNUC__) && !defined(__NO_INLINE__) && !defined(__clang__)
#define __static extern
#else
#define __static static
#endif

#if !defined(__GNUC__) || (__GNUC__ < 3)
#define __builtin_expect(foo,bar) (foo)
#define __expect(foo,bar) (foo)
#else
#define __expect(foo,bar) __builtin_expect((long)(foo),bar)
#endif

#if defined(__GNUC__) && !defined(__likely)
#define __likely(foo) __expect((foo),1)
#define __unlikely(foo) __expect((foo),0)
#endif

#if !defined(__likely)
#define __likely(foo) (foo)
#define __unlikely(foo) (foo)
#endif

/* return 0 for range error / overflow, 1 for ok */

/* we assume the normal case is that the checked value is in range */

/* does ptr point to one of buf[0], buf[1], ... buf[len-1]? */
__static inline __gnuinline int range_ptrinbuf(const void* buf,size_t len,const void* ptr) {
  register const char* c=(const char*)buf;	/* no pointer arithmetic on void* */
  return __likely(c &&		/* is buf non-NULL? */
	  ((uintptr_t)c)+len>(uintptr_t)c &&	/* gcc 4.1 miscompiles without (uintptr_t) */
			/* catch integer overflows and fail if buffer is 0 bytes long */
			/* because then ptr can't point _in_ the buffer */
	  (uintptr_t)((const char*)ptr-c)<len);	/* this one is a little tricky.
     "ptr-c" checks the offset of ptr in the buffer is inside the buffer size.
     Now, ptr-c can underflow; say it is -1.  When we cast it to uintptr_t, it becomes
     a very large number. */
}

/* same thing, but the buffer is specified by a pointer to the first
 * byte (Min) and a pointer after the last byte (Max). */
__static inline __gnuinline int range_ptrinbuf2(const void* Min,const void* Max,const void* ptr) {
  return __likely(Min && ptr>=Min && ptr<Max);
  /* Min <= Max is implicitly checked here */
}

/* Is this a plausible buffer?
 * Check whether buf is NULL, and whether buf+len overflows.
 * Does NOT check whether buf has a non-zero length! */
__static inline __gnuinline int range_validbuf(const void* buf,size_t len) {
  return __likely(buf && (uintptr_t)buf+len>=(uintptr_t)buf);
}

/* same thing but buffer is given as pointer to first byte (Min) and
 * pointer beyond last byte (Max).  Again, an 0-size buffer is valid. */
__static inline __gnuinline int range_validbuf2(const void* Min,const void* Max) {
  return __likely(Min && Max>=Min);
}

/* is buf2[0..len2-1] inside buf1[0..len-1]? */
__static inline __gnuinline int range_bufinbuf(const void* buf1,size_t len1,const void* buf2,size_t len2) {
  return range_validbuf(buf1,len1) &&
         range_validbuf(buf2,len2) &&
	 __likely(buf1<=buf2 &&
	 (ptrdiff_t)buf1+len1>=(ptrdiff_t)buf2+len2);
}

/* does an array of "elements" members of size "membersize" starting at
 * "arraystart" lie inside buf1[0..len-1]? */
att_const
int range_arrayinbuf(const void* buf,size_t len,
		     const void* arraystart,size_t elements,size_t membersize);

/* does an ASCIIZ string starting at "ptr" lie in buf[0..len-1]? */
att_const
int range_strinbuf(const void* buf,size_t len,const void* stringstart);

/* does an UTF-16 string starting at "ptr" lie in buf[0..len-1]? */
att_const
int range_str2inbuf(const void* buf,size_t len,const void* stringstart);

/* does an UTF-32 string starting at "ptr" lie in buf[0..len-1]? */
att_const
int range_str4inbuf(const void* buf,size_t len,const void* stringstart);


/* I originally omitted addition and substraction because it appeared
 * trivial.  You could just add the two numbers and see if it was
 * smaller than either of them.  This always worked for me because I
 * only cared about unsigned arithmetic, but for signed arithmetic,
 * adding two numbers is undefined if the result does not fit in the
 * int.  gcc has started to actually use this undefinedness to screw
 * you.  The following code illustrates this:
 *   int a=INT_MAX,b=a+5;
 *   if (b<a) abort();  // whole statement optimized away by gcc 4.1
 *   // at this point, b<a
 * So I decided to add some integer overflow protection functionality
 * here for addition and subtraction, too. */

/* usage:
 * if (add_of(dest,a,b)) return EINVAL;		// dest=a+b;
 * if (sub_of(dest,a,b)) return EINVAL;		// dest=a-b;
 * if (assign(dest,some_int)) return EINVAL;	// dest=some_int;
 */

/* two important assumptions:
 *   1. the platform is using two's complement
 *   2. there are 8 bits in a byte
 */

#define __HALF_MAX_SIGNED(type) ((type)1 << (sizeof(type)*8-2))
#define __MAX_SIGNED(type) (__HALF_MAX_SIGNED(type) - 1 + __HALF_MAX_SIGNED(type))
#define __MIN_SIGNED(type) (-1 - __MAX_SIGNED(type))

/* we use <1 and not <0 to avoid a gcc warning */
#define __MIN(type) ((type)-1 < 1?__MIN_SIGNED(type):(type)0)
#define __MAX(type) ((type)~__MIN(type))

#define assign(dest,src) ({ typeof(src) __x=(src); typeof(dest) __y=__x; (__x==__y && ((__x<1) == (__y<1))?(void)((dest)=__y),0:1); })

/* gcc 5 now has nice builtins we can use instead */
#if defined(__GNUC__) && (__GNUC__ >= 5)

#define add_of(c,a,b) __builtin_add_overflow(a,b,&c)
#define sub_of(c,a,b) __builtin_sub_overflow(a,b,&c)

#else

/* if a+b is defined and does not have an integer overflow, do c=a+b and
 * return 0.  Otherwise, return 1. */
#define add_of(c,a,b) ({ typeof(a) __a=a; typeof(b) __b=b; (__b)<1?((__MIN(typeof(a+b))-(__b)<=(__a))?assign(c,__a+__b):1) : ((__MAX(typeof(c))-(__b)>=(__a))?assign(c,__a+__b):1); })

#define sub_of(c,a,b) ({ typeof(a) __a=a; typeof(b) __b=b; (__b)<1?((__MAX(typeof(a+b))+__b>=__a)?assign(c,__a-__b):1) : ((__MIN(typeof(c))+__b<=__a)?assign(c,__a-__b):1); })

#endif

#undef __static

#ifdef __cplusplus
}
#endif

#endif

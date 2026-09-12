#ifndef CAS_H
#define CAS_H

#include <stddef.h>

/* Atomic operations for lock-free data structures.
 * We operate on machine words and use size_t as a type.
 * CAS stands for Compare And Swap, the most common operation. */

/* The API was inspired by the Linux kernel */

#if defined(__INTEL_COMPILER) || (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 1))
#define USE_BUILTINS
#endif

/* if (*x == oldval) { *x=newval; return 1; } else return 0; */
static inline int compare_and_swap(volatile size_t* x,size_t oldval,size_t newval) {
#ifdef USE_BUILTINS
  return __sync_bool_compare_and_swap(x,oldval,newval);
#elif defined(__i386__)
  char result;
  asm volatile ("lock; cmpxchgl %3, %0; setz %1" : "=m"(*x), "=q" (result) : "m" (*x), "r" (newval), "a" (oldval) : "memory");
  return result;
#elif defined(__x86_64__)
  char result;
  asm volatile ("lock; cmpxchgq %3, %0; setz %1" : "=m"(*x), "=q" (result) : "m" (*x), "r" (newval), "a" (oldval) : "memory");
  return result;
#else
#error architecture not supported and gcc too old, edit CAS.h
#endif
}

/* return *x += val; */
static inline size_t atomic_add_return(size_t* x,size_t val) {
#ifdef USE_BUILTINS
  return __sync_add_and_fetch(x,val);
#elif defined(__i386__)
  size_t i = val;
  asm volatile ("lock; xaddl %1, %0" : "+m" (*x), "+r" (val) :: "memory" );
  return i + val;
#elif defined(__x86_64__)
  size_t i = val;
  asm volatile ("lock; xaddq %1, %0" : "+m" (*x), "+r" (val) :: "memory" );
  return i + val;
#else
  size_t y;
  for (y=*x; compare_and_swap(&x,y,y+val)==0; y=*x) ;
  return y+val;
#endif
}


/* *x += val; */
static inline void atomic_add(size_t* x,size_t val) {
#ifdef USE_BUILTINS
  __sync_add_and_fetch(x,val);
#elif defined(__i386__)
  asm volatile ("lock; addl %1, %0" : "+m" (*x) : "ir" (val) );
#elif defined(__x86_64__)
  asm volatile ("lock; addq %1, %0" : "+m" (*x) : "ir" (val) );
#else
  atomic_add_return(&x,val);
#endif
}

static inline void atomic_inc(size_t* x) {
#ifdef __i386__
  asm volatile ("lock; incl %0" : "+m" (*x) );
#elif defined(__x86_64__)
  asm volatile ("lock; incq %0" : "+m" (*x) );
#else
  atomic_add(x,1);
#endif
}

static inline size_t atomic_inc_return(size_t* x) {
  return atomic_add_return(x,1);
}

static inline void atomic_dec(size_t* x) {
#ifdef __i386__
  asm volatile ("lock; decl %0" : "+m" (*x) );
#elif defined(__x86_64__)
  asm volatile ("lock; decq %0" : "+m" (*x) );
#else
  atomic_add(x,-1);
#endif
}

static inline size_t atomic_dec_return(size_t* x) {
  return atomic_add_return(x,-1);
}

/* *x |= val; */
static inline void atomic_or(volatile size_t* x,size_t val) {
#ifdef USE_BUILTINS
  __sync_or_and_fetch(x,val);
#elif defined(__i386__)
  asm volatile ("lock; orl %1, %0" : "+m" (*x) : "r" (val) );
#elif defined(__x86_64__)
  asm volatile ("lock; orq %1, %0" : "+m" (*x) : "r" (val) );
#else
#error architecture not supported and gcc too old, edit CAS.h
#endif
}

/* *x &= val; */
static inline void atomic_and(volatile size_t* x,size_t val) {
#ifdef USE_BUILTINS
  __sync_and_and_fetch(x,val);
#elif defined(__i386__)
  asm volatile ("lock; andl %1, %0" : "+m" (*x) : "r" (val) );
#elif defined(__x86_64__)
  asm volatile ("lock; andq %1, %0" : "+m" (*x) : "r" (val) );
#else
#error architecture not supported and gcc too old, edit CAS.h
#endif
}

/* Optimization barrier */
/* The "volatile" is due to gcc bugs */
#define barrier() __asm__ __volatile__("": : :"memory")

#if defined(__i386__) || defined(__x86_64__)

#define mb()	asm volatile("mfence":::"memory")
#define rmb()	asm volatile("lfence":::"memory")
#define wmb()	asm volatile("sfence":::"memory")

/* Atomic operations are already serializing on x86 */
#define smp_mb__before_atomic_dec()	barrier()
#define smp_mb__after_atomic_dec()	barrier()
#define smp_mb__before_atomic_inc()	barrier()
#define smp_mb__after_atomic_inc()	barrier()

#elif defined(__powerpc__)

#define mb()	asm volatile("sync":::"memory")
#define rmb()	asm volatile("sync":::"memory")
#define wmb()	asm volatile("sync":::"memory")

#define smp_mb__before_atomic_dec()     mb()
#define smp_mb__after_atomic_dec()      mb()
#define smp_mb__before_atomic_inc()     mb()
#define smp_mb__after_atomic_inc()      mb()

#elif defined(USE_BUILTINS)

#define mb()	__sync_synchronize()
#define rmb()	__sync_synchronize()
#define wmb()	__sync_synchronize()

#endif

#undef USE_BUILTINS

#endif


#ifdef __clang__

// clang already includes the condition in the error message and emits
// an additional warning here if we use the variant without message
#define compiletimeassert(cond) _Static_assert(cond,"")

#elif __STDC_VERSION__ >= 201710L

#define compiletimeassert(cond) _Static_assert(cond,#cond)

#elif __STDC_VERSION__ >= 201112L

#define compiletimeassert(cond) _Static_assert(cond,#cond)

#else

#define __X(x, y) x ## y
#define __Y(x, y) __X(x, y)
#define compiletimeassert(cond) struct __Y(foo,__LINE__) { char __temp[1 - (!(cond))*2]; };

#endif

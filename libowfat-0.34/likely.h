#ifdef __dietlibc__
#include <sys/cdefs.h>
#else

#if __GNUC__ < 3
#define __expect(foo,bar) (foo)
#else
#define __expect(foo,bar) __builtin_expect((long)(foo),bar)
#endif
#define __likely(foo) __expect((foo),1)
#define __unlikely(foo) __expect((foo),0)

#endif

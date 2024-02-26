
#define __X(x, y) x ## y
#define __Y(x, y) __X(x, y)
#define compiletimeassert(cond) struct __Y(foo,__LINE__) { char __temp[1 - (!(cond))*2]; };

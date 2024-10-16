#ifndef LIBOWFAT_ATTRIBUTES_H
#define LIBOWFAT_ATTRIBUTES_H

#ifndef __GNUC__
// macro attribute declarations away if we don't have gcc or clang
#define __attribute__(x)
#define __extension__
#endif

#ifndef __cplusplus
#define noexcept
#endif

#define GCC_VERSION_ATLEAST(a,b) (__GNUC__ > a) || ((__GNUC__ == a) && (__GNUC_MINOR__ >= b))

#if GCC_VERSION_ATLEAST(2,95) && !defined(__STRICT_ANSI__)
#undef restrict
#else
#define restrict
#endif

#if GCC_VERSION_ATLEAST(3,0)
#define likely(a) __builtin_expect((a), 1)
#define unlikely(a) __builtin_expect((a), 0)
#else
#define likely(a) (a)
#define unlikely(a) (a)
#endif

#if GCC_VERSION_ATLEAST(2, 5)
#define att_const __attribute__((__const__))
#else
#define att_const
#endif

#if GCC_VERSION_ATLEAST(2, 96)
#define att_pure __attribute__((__pure__))
#else
#define att_pure
#endif

#if GCC_VERSION_ATLEAST(3, 0)
#define att_malloc __attribute__((__malloc__))
#else
#define att_malloc
#endif

#if GCC_VERSION_ATLEAST(3, 4)
#define att_warn_unused_result __attribute__((__warn_unused_result__))
#else
#define att_warn_unused_result
#endif

#if GCC_VERSION_ATLEAST(4, 3)
#define att_hot __attribute__((__hot__))
#define att_cold __attribute__((__cold__))
#else
#define att_hot
#define att_cold
#endif

#if GCC_VERSION_ATLEAST(4, 3)
#define att_alloc(x) __attribute__((alloc_size(x)))
#define att_calloc(x,y) __attribute__((alloc_size(x,y)))
#else
#define att_alloc
#define att_calloc
#endif

#if GCC_VERSION_ATLEAST(10, 0)
#define att_read(argno_ptr) __attribute__((access(read_only, argno_ptr)))
#define att_readn(argno_ptr, argno_size) __attribute__((access(read_only, argno_ptr, argno_size)))
#define att_write(argno_ptr) __attribute__((access(write_only, argno_ptr)))
#define att_writen(argno_ptr, argno_size) __attribute__((access(write_only, argno_ptr, argno_size)))
#define att_mangle(argno_ptr) __attribute__((access(read_write, argno_ptr)))
#define att_manglen(argno_ptr, argno_size) __attribute__((access(read_write, argno_ptr, argno_size)))
#else
#define att_read(argno_ptr)
#define att_readn(argno_ptr, argno_size)
#define att_write(argno_ptr)
#define att_writen(argno_ptr, argno_size)
#define att_mangle(argno_ptr)
#define att_manglen(argno_ptr, argno_size)
#endif

#if GCC_VERSION_ATLEAST(3, 2)
#define att_dontuse __attribute__((__deprecated__))
#else
#define att_dontuse
#endif

#if GCC_VERSION_ATLEAST(3, 3)
#define att_nonnull(params) __attribute__((__nonnull__(params)))
#else
#define att_nonnull(params)
#endif

#if GCC_VERSION_ATLEAST(4, 3)
#define att_warn(message) __attribute__((__warning__(message)))
#define att_error(message) __attribute__((__warning__(message)))
#else
#define att_warn(message)
#define att_error(message)
#endif

#ifndef __GNUC__
#define __builtin_constant_p(x) 0
#endif

#endif

#pragma once

#include "reef-macro.h"
#include <cstdint>

namespace reef
{

typedef std::uint8_t u8; /**< 8-bit unsigned integer */
typedef std::uint16_t u16; /**< 16-bit unsigned integer */
typedef std::uint32_t u32; /**< 32-bit unsigned integer */
typedef std::uint64_t u64; /**< 64-bit unsigned integer */

typedef std::int8_t i8; /**< 8-bit signed integer */
typedef std::int16_t i16; /**< 16-bit signed integer */
typedef std::int32_t i32; /**< 32-bit signed integer */
typedef std::int64_t i64; /**< 64-bit signed integer */

#if defined(__x86_64__)
typedef u64 usize; /**< pointer-sized unsigned integer */
typedef i64 isize; /**< pointer-sized signed integer */

#elif defined(__i386__)
typedef u32 usize; /**< pointer-sized unsigned integer */
typedef i32 isize; /**< pointer-sized signed integer */

#else
#error "reef-base.h: Unsupported platform"
#endif

/** RAII guard that runs a callable when it leaves scope. */
template <typename F> struct Defer
{
    F f;
    ~Defer() { f(); }
};

/** Wraps `f` in a Defer guard; prefer the reef_defer macro. */
template <typename F> inline Defer<F> defer(F f) { return Defer<F> { f }; }

/** Runs `__code` at the end of the enclosing scope. */
#define reef_defer(__code)                                                     \
    auto REEF_SYM(_defer_) = reef::defer([&]() { __code; })

/** Runs `__code` at scope exit only when `__err_cond` is true (error-path
 * cleanup). */
#define reef_err_defer(__err_cond, __code)                                     \
    auto REEF_SYM(_defer_) = reef::defer(                                      \
        [&]()                                                                  \
        {                                                                      \
            if (__err_cond)                                                    \
            {                                                                  \
                __code;                                                        \
            }                                                                  \
        })

} // namespace reef

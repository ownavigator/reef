#pragma once

#include "reef-macro.h"
#include <cstdint>
#include <type_traits>

namespace reef
{

typedef std::uint8_t u8;
typedef std::uint16_t u16;
typedef std::uint32_t u32;
typedef std::uint64_t u64;

typedef std::int8_t i8;
typedef std::int16_t i16;
typedef std::int32_t i32;
typedef std::int64_t i64;

#if defined(__x86_64__)
typedef u64 usize;
typedef i64 isize;

#elif defined(__i386__)
typedef u32 usize;
typedef i32 isize;

#else
#error "reef-base.h: Unsupported platform"
#endif

/** An RAII guard that runs a callable when it leaves scope. */
template <typename F> struct Defer
{
    static_assert(std::is_invocable_v<F>, "Defer: F must be callable");

    F f; /**< Holds the callable to run at scope exit. */
    ~Defer() { f(); }
};

template <typename F> inline Defer<F> defer(F f) { return Defer<F> { f }; }

/** Runs `__code` at the end of the enclosing scope. */
#define reef_defer(__code)                                                     \
    auto REEF_SYM(_defer_) = reef::defer([&]() { __code; })

/** Runs `__code` at scope exit, but only when `__err_cond` holds, for
 * error-path cleanup. */
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

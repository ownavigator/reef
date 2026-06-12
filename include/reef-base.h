#pragma once

#include "reef-macro.h"
#include <cstdint>

namespace reef {
/*
 * Unsigned fixed-integer types
 */

typedef std::uint8_t u8;
typedef std::uint16_t u16;
typedef std::uint32_t u32;
typedef std::uint64_t u64;

/*
 * Signed fixed-integer types
 */

typedef std::int8_t i8;
typedef std::int16_t i16;
typedef std::int32_t i32;
typedef std::int64_t i64;

/*
 * Platform-specific types
 */

#if defined(__x86_64__)
typedef u64 usize;
typedef i64 isize;

#elif defined(__i386__)
typedef u32 usize;
typedef i32 isize;

#else
#error "reef-base.h: Unsupported platform"
#endif

/*
 * Defer operations to the scope.
 */

template <typename F> struct Defer {
    F f;
    ~Defer() { f(); }
};

template <typename F> inline Defer<F> defer(F f) { return Defer<F> { f }; }

#define reef_defer(__code)                                                     \
    auto REEF_SYM(_defer_) = reef::defer([&]() { __code; })

#define reef_err_defer(__err_cond, __code)                                     \
    auto REEF_SYM(_defer_) = reef::defer([&]() {                               \
        if (__err_cond) {                                                      \
            __code;                                                            \
        }                                                                      \
    })

} // namespace reef

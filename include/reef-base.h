#pragma once

#include <cstdint>
#include <cstring>
#include <type_traits>

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
 * Allocator type
 */

struct Allocator {
    virtual u8* alloc(usize size, usize align) = 0;
    virtual void dealloc(u8* ptr, usize size, usize align) = 0;
};

template <typename T>
inline T* alloc(Allocator* allocator, usize count = 1)
{
    return (T*)allocator->alloc(sizeof(T) * count, alignof(T));
}

template <typename T>
inline void dealloc(Allocator* allocator, T* ptr, usize count = 1)
{
    allocator->dealloc((u8*)ptr, sizeof(T) * count, alignof(T));
}

template <typename T>
inline T* copy(Allocator* allocator, const T* src, usize count = 1)
{
    static_assert(std::is_trivially_copyable_v<T>,
        "copy: T must be trivially copyable");

    T* dst = alloc<T>(allocator, count);
    std::memcpy(dst, src, sizeof(T) * count);
    return dst;
}

/*
 * Unicode string types
 */

struct Wide_Str {
    i16* ptr;
    usize len;
};

inline Wide_Str wide_str_nocopy_new(i16* ptr, usize len)
{
    return Wide_Str { ptr, len };
}

bool wide_str_from_utf8(Wide_Str* uninit, u8 const* str, usize str_len,
    Allocator* allocator);

void wide_str_drop(Wide_Str* str, Allocator* allocator);

} // namespace reef

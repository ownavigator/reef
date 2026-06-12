#pragma once

#include "reef-base.h"
#include <cstring>
#include <type_traits>

namespace reef
{

/** Abstract allocator interface. */
struct Allocator
{
    /** Allocate `size` bytes with `align` alignment. */
    virtual u8* alloc(usize size, usize align) = 0;
    /** Free a block previously returned by alloc. */
    virtual void dealloc(u8* ptr, usize size, usize align) = 0;
};

/** Allocates `count` instances of `T` from `allocator`. */
template <typename T> inline T* alloc(Allocator* allocator, usize count)
{
    return (T*)allocator->alloc(sizeof(T) * count, alignof(T));
}

/** Frees memory from a matching `alloc<T>`. */
template <typename T>
inline void dealloc(Allocator* allocator, T* ptr, usize count)
{
    allocator->dealloc((u8*)ptr, sizeof(T) * count, alignof(T));
}

/** Allocates and copies `count` trivially-copyable `T` from `src`. */
template <typename T>
inline T* copy(Allocator* allocator, const T* src, usize count)
{
    static_assert(
        std::is_trivially_copyable_v<T>, "copy: T must be trivially copyable");

    T* dst = alloc<T>(allocator, count);
    std::memcpy(dst, src, sizeof(T) * count);
    return dst;
}

/** Returns the process-wide heap allocator. */
Allocator* heap_allocator();

}
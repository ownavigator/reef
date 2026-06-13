#pragma once

#include "reef-base.h"
#include <cstring>
#include <type_traits>

namespace reef
{

/** A zero-data vtable that hands out and reclaims raw, aligned memory.
 *  Every allocating function takes one explicitly, so allocation is never
 *  implicit and an owned type never stores the allocator it was made with. */
struct Allocator
{
    /** Returns `size` bytes aligned to `align`, or null on failure. */
    virtual u8* alloc(usize size, usize align) = 0;
    /** Reclaims a block previously returned by alloc with the same size and
     *  align. */
    virtual void dealloc(u8* ptr, usize size, usize align) = 0;
};

template <typename T> inline T* alloc(Allocator* allocator, usize count)
{
    return (T*)allocator->alloc(sizeof(T) * count, alignof(T));
}

template <typename T>
inline void dealloc(Allocator* allocator, T* ptr, usize count)
{
    allocator->dealloc((u8*)ptr, sizeof(T) * count, alignof(T));
}

template <typename T>
inline T* copy(Allocator* allocator, const T* src, usize count)
{
    static_assert(
        std::is_trivially_copyable_v<T>, "copy: T must be trivially copyable");

    T* dst = alloc<T>(allocator, count);
    std::memcpy(dst, src, sizeof(T) * count);
    return dst;
}

Allocator* heap_allocator();

}
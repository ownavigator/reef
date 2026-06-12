#pragma once

#include "reef-base.h"
#include <cstring>
#include <type_traits>

namespace reef {

/*
 * Allocator type
 */

struct Allocator {
    virtual u8* alloc(usize size, usize align) = 0;
    virtual void dealloc(u8* ptr, usize size, usize align) = 0;
};

template <typename T> inline T* alloc(Allocator* allocator, usize count = 1)
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
    static_assert(
        std::is_trivially_copyable_v<T>, "copy: T must be trivially copyable");

    T* dst = alloc<T>(allocator, count);
    std::memcpy(dst, src, sizeof(T) * count);
    return dst;
}

Allocator* heap_allocator();

}
#include "reef-allocator.h"
#include <new>

namespace reef {

struct Heap_Allocator : Allocator {
    u8* alloc(usize size, usize align) override
    {
        return static_cast<u8*>(
            ::operator new[](size, std::align_val_t(align), std::nothrow));
    }

    void dealloc(u8* ptr, usize, usize align) override
    {
        ::operator delete[](ptr, std::align_val_t(align), std::nothrow);
    }

    static Heap_Allocator* instance()
    {
        static Heap_Allocator ALLOCATOR { };
        return &ALLOCATOR;
    }
};

Allocator* heap_allocator() { return Heap_Allocator::instance(); }

}
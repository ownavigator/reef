#pragma once

#include "reef-allocator.h"
#include "reef-base.h"
#include <cassert>

namespace reef
{

/** A UTF-8 string whose buffer the caller owns. */
struct Str
{
    u8* ptr; /**< Points at the owned UTF-8 code units. */
    usize len; /**< Counts the buffer length in u8 code units. */
};

inline Str str_new_nocopy(u8* ptr, usize len) { return Str { ptr, len }; }

bool str_new(Str* uninit, u8 const* str, usize str_len, Allocator* allocator);

/** Decodes the codepoint at `idx`, returning the count of u8 units consumed or
 * a negative utf8proc error code. */
isize str_at(Str const* str, usize idx, i32* out_codepoint);

void str_drop(Str* str, Allocator* allocator);

/** A UTF-16 string whose buffer the caller owns. */
struct Wide_Str
{
    u16* ptr; /**< Points at the owned UTF-16 code units. */
    usize len; /**< Counts the buffer length in u16 code units. */
};

inline Wide_Str wide_str_new_nocopy(u16* ptr, usize len)
{
    return Wide_Str { ptr, len };
}

bool wide_str_from_utf8(
    Wide_Str* uninit, u8 const* str, usize str_len, Allocator* allocator);

/** Decodes the codepoint at `idx`, returning the count of u16 units consumed or
 * a negative utf8proc error code. */
isize wide_str_at(Wide_Str const* str, usize idx, i32* out_codepoint);

void wide_str_drop(Wide_Str* str, Allocator* allocator);

}

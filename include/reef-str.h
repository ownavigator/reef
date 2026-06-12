#pragma once

#include "reef-allocator.h"
#include "reef-base.h"
#include <cassert>

namespace reef {

/*
 * Unicode string types
 */

struct Wide_Str {
    u16* ptr;
    usize len;
};

inline Wide_Str wide_str_nocopy_new(u16* ptr, usize len)
{
    return Wide_Str { ptr, len };
}

bool wide_str_from_utf8(
    Wide_Str* uninit, u8 const* str, usize str_len, Allocator* allocator);

isize wide_str_at(Wide_Str const* str, usize idx, i32* codepoint_ref);

/*
 * Note: Caller ensures that the string is dropable.
 */

void wide_str_drop(Wide_Str* str, Allocator* allocator);

}
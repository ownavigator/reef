#pragma once

#include "reef-allocator.h"
#include "reef-base.h"
#include <cassert>

namespace reef
{

/** A UTF-8 string. Caller owns the buffer. */
struct Str
{
    u8* ptr; /**< UTF-8 code units */
    usize len; /**< length in u8 units */
};

/** Wraps an existing u8 buffer without copying.
 *  Caller ensures the pointer is valid and the string is UTF-8.
 */
inline Str str_new_nocopy(u8* ptr, usize len) { return Str { ptr, len }; }

/** Copies an existing UTF-8 buffer. */
bool str_new(Str* uninit, u8 const* str, usize str_len, Allocator* allocator);

/** Decodes the codepoint at `idx` into `out_codepoint`; returns the number of
 * u8 units consumed, or a negative utf8proc error code.
 */
isize str_at(Str const* str, usize idx, i32* out_codepoint);

/** Frees the buffer owned by `str`. Caller ensures the string is dropable. */
void str_drop(Str* str, Allocator* allocator);

/** A UTF-16 string. Caller owns the buffer. */
struct Wide_Str
{
    u16* ptr; /**< UTF-16 code units */
    usize len; /**< length in u16 units */
};

/** Wraps an existing u16 buffer without copying.
 *  Caller ensures the pointer is valid and the string is UTF-16.
 */
inline Wide_Str wide_str_new_nocopy(u16* ptr, usize len)
{
    return Wide_Str { ptr, len };
}

/** Converts a UTF-8 string to UTF-16, allocating into `uninit`. Returns false
 * on allocation/conversion failure. */
bool wide_str_from_utf8(
    Wide_Str* uninit, u8 const* str, usize str_len, Allocator* allocator);

/** Decodes the codepoint at `idx` into `out_codepoint`; returns the number of
 * u16 units consumed, or a negative utf8proc error code. */
isize wide_str_at(Wide_Str const* str, usize idx, i32* out_codepoint);

/** Frees the buffer owned by `str`. Caller ensures the string is dropable. */
void wide_str_drop(Wide_Str* str, Allocator* allocator);

}
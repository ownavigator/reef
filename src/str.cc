#include "reef-str.h"
#include "simdutf.h" // IWYU pragma: keep
#include "utf8proc.h"
#include <cassert>

namespace reef
{

bool wide_str_from_utf8(
    Wide_Str* uninit, u8 const* str, usize str_len, Allocator* allocator)
{

    bool ok = false;

    usize wide_len = simdutf::utf16_length_from_utf8((char const*)str, str_len);
    u16* wide_ptr = alloc<u16>(allocator, wide_len);
    reef_err_defer(!ok, dealloc(allocator, wide_ptr, wide_len));

    isize rc = simdutf::convert_utf8_to_utf16(
        (char const*)str, str_len, (char16_t*)wide_ptr);

    if (rc < 0)
    {
        return false;
    }

    *uninit = wide_str_new_nocopy(wide_ptr, (usize)rc);
    ok = true;
    return ok;
}

isize wide_str_at(Wide_Str const* str, usize idx, i32* out_codepoint)
{
    assert(idx < str->len);
    return utf8proc_iterate_utf16(
        str->ptr + idx, str->len - idx, out_codepoint);
}

void wide_str_drop(Wide_Str* str, Allocator* allocator)
{
    dealloc(allocator, str->ptr, str->len);
}

} // namespace reef
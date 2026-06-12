
#include "reef-allocator.h"
#include "reef-str.h"
#include <cstdio>

static char const* JS_SOURCE = R"(
function add(😀, b) {
    return  + b;
}
)";

int main()
{
    reef::Wide_Str str;
    if (!reef::wide_str_from_utf8(&str, (reef::u8*)JS_SOURCE,
            std::strlen(JS_SOURCE), reef::heap_allocator()))
    {
        return 1;
    }

    for (reef::usize i = 0; i < str.len;)
    {
        reef::i32 codepoint;

        reef::isize advance = reef::wide_str_at(&str, i, &codepoint);
        if (advance < 0)
        {
            std::fprintf(stderr, "Offset %zu: error %zd\n", i, advance);
            i += 1;
            continue;
        }

        std::fprintf(stderr, "Offset %zu: U+%04X\n", i, codepoint);
        i += (reef::usize)advance;
    }

    reef::wide_str_drop(&str, reef::heap_allocator());
}
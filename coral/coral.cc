
#include "reef-allocator.h"
#include "reef-str.h"
#include <cstdio>

static char const* JS_SOURCE = R"(
function add(😀, b) {
    return 😀 + b;
}
)";

int main()
{
    reef::Wide_Str str;
    if (!reef::wide_str_from_utf8(&str, (reef::u8*)JS_SOURCE,
            std::strlen(JS_SOURCE), reef::heap_allocator())) {
        return 1;
    }

    for (reef::usize i = 0; i < str.len;) {
        reef::i32 codepoint;

        reef::usize offset = i;
        i += reef::wide_str_at(&str, i, &codepoint);

        std::fprintf(stderr, "Offset %zu: U+%04X\n", offset, codepoint);
    }

    reef::wide_str_drop(&str, reef::heap_allocator());
}
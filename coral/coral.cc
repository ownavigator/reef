
#include "reef-allocator.h"
#include "reef-module.h"
#include "reef-str.h"
#include <cstdio>

static char const* JS_PATH = "example.js";

static char const* JS_SOURCE = R"(
function add(😀, b) {
    return  + b;
}
)";

int main()
{
    reef::Wide_Str path;
    if (!reef::wide_str_from_utf8(&path, (reef::u8*)JS_PATH,
            std::strlen(JS_PATH), reef::heap_allocator()))
    {
        return 1;
    }

    reef::Wide_Str content;
    if (!reef::wide_str_from_utf8(&content, (reef::u8*)JS_SOURCE,
            std::strlen(JS_SOURCE), reef::heap_allocator()))
    {
        return 1;
    }

    reef::Module module = reef::module_new(path, content);
    printf("Module path: %.*s\n", (int)module.path.len,
        (const char*)module.path.ptr);
    printf("Module content: %.*s\n", (int)module.content.len,
        (const char*)module.content.ptr);
    reef::module_drop(&module, reef::heap_allocator());
}
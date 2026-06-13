#include "reef-module.h"

namespace reef
{

void module_drop(Module* module, Allocator* allocator)
{
    wide_str_drop(&module->path, allocator);
    wide_str_drop(&module->content, allocator);
}

}
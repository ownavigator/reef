#pragma once

#include "reef-str.h"
namespace reef
{

/** The JavaScript module */
struct Module
{
    Wide_Str path;
    Wide_Str content;
};

inline Module module_new(Wide_Str path, Wide_Str content)
{
    return Module { path, content };
}

void module_drop(Module* module, Allocator* allocator);

}
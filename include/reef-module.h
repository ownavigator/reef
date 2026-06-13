#pragma once

#include "reef-str.h"
namespace reef
{

/** A JavaScript source file and its compilation state. */
struct Module
{
    Wide_Str path; /**< Owns the path the source was loaded from. */
    Wide_Str content; /**< Owns the decoded UTF-16 source text. */
};

inline Module module_new(Wide_Str path, Wide_Str content)
{
    return Module { path, content };
}

void module_drop(Module* module, Allocator* allocator);

}